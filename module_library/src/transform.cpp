// Copyright 2015 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "module_library/transform.h"

#include <string>

#include "breadboard/base_node.h"
#include "breadboard/module_registry.h"
#include "corgi/entity_manager.h"
#include "corgi_component_library/transform.h"
#include "mathfu/glsl_mappings.h"

using breadboard::BaseNode;
using breadboard::ModuleRegistry;
using breadboard::Module;
using breadboard::NodeArguments;
using breadboard::NodeSignature;
using corgi::component_library::TransformComponent;
using corgi::component_library::TransformData;
using corgi::EntityRef;
using mathfu::vec3;

namespace breadboard {
namespace module_library {

// Returns the child at the given index.
class ChildNode : public BaseNode {
 public:
  enum { kInputParent, kInputChildIndex };
  enum { kOutputChild };

  ChildNode(TransformComponent* transform_component)
      : transform_component_(transform_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<EntityRef>(kInputParent, "Parent");
    node_sig->AddInput<int>(kInputChildIndex, "Child Index");
    node_sig->AddOutput<EntityRef>(kOutputChild, "Child");
  }

  virtual void Initialize(NodeArguments* args) {
    auto entity = args->GetInput<EntityRef>(kInputParent);
    if (entity->IsValid()) {
      auto child_index = *args->GetInput<int>(kInputChildIndex);
      TransformData* transform_data =
          transform_component_->GetComponentData(*entity);
      // Find the child at the given index.
      auto iter = transform_data->children.begin();
      while (child_index && iter != transform_data->children.end()) {
        ++iter;
        --child_index;
      }
      args->SetOutput(kOutputChild, iter != transform_data->children.end()
                                        ? iter->owner
                                        : EntityRef());
    } else {
      args->SetOutput(kOutputChild, EntityRef());
    }
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }

 private:
  TransformComponent* transform_component_;
};

// Returns the position of the entity in world space.
class WorldPositionNode : public BaseNode {
 public:
  enum { kInputTrigger, kInputEntity };
  enum { kOutputPosition };
  WorldPositionNode(TransformComponent* transform_component)
      : transform_component_(transform_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>(kInputTrigger, "Trigger");
    node_sig->AddInput<EntityRef>(kInputEntity, "Entity");
    node_sig->AddOutput<vec3>(kOutputPosition, "Position");
  }

  virtual void Initialize(NodeArguments* args) {
    if (args->IsInputDirty(kInputTrigger)) {
      auto entity = args->GetInput<EntityRef>(kInputEntity);
      vec3 position = transform_component_->WorldPosition(*entity);
      args->SetOutput(kOutputPosition, position);
    }
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }

 private:
  TransformComponent* transform_component_;
};

class SetScaleNode : public BaseNode {
 public:
  enum { kInputEntity, kInputScale };
  SetScaleNode(TransformComponent* transform_component)
      : transform_component_(transform_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<EntityRef>(kInputEntity, "Entity");
    node_sig->AddInput<vec3>(kInputScale, "Scale");
  }

  virtual void Initialize(NodeArguments* args) {
    auto entity = args->GetInput<EntityRef>(kInputEntity);
    if (entity->IsValid()) {
      TransformData* transform_data =
          transform_component_->GetComponentData(*entity);
      transform_data->scale = *args->GetInput<vec3>(kInputScale);
    }
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }

 private:
  TransformComponent* transform_component_;
};

void InitializeTransformModule(ModuleRegistry* module_registry,
                               TransformComponent* transform_component) {
  auto world_position_ctor = [transform_component]() {
    return new WorldPositionNode(transform_component);
  };
  auto set_scale_ctor = [transform_component]() {
    return new SetScaleNode(transform_component);
  };
  auto child_ctor = [transform_component]() {
    return new ChildNode(transform_component);
  };
  Module* module = module_registry->RegisterModule("transform");
  module->RegisterNode<ChildNode>("child", child_ctor);
  module->RegisterNode<WorldPositionNode>("world_position",
                                          world_position_ctor);
  module->RegisterNode<SetScaleNode>("set_scale", set_scale_ctor);
}

}  // namespace module_library
}  // namespace breadboard
