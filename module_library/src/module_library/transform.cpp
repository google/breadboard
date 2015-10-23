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
#include "component_library/transform.h"
#include "entity/entity_manager.h"
#include "mathfu/glsl_mappings.h"

using breadboard::BaseNode;
using breadboard::ModuleRegistry;
using breadboard::Module;
using breadboard::NodeArguments;
using breadboard::NodeSignature;
using breadboard::TypeRegistry;
using fpl::component_library::TransformComponent;
using fpl::component_library::TransformData;
using fpl::entity::EntityRef;
using mathfu::vec3;

namespace fpl {
namespace module_library {

// Returns the transform component data of the given entity.
class TransformNode : public BaseNode {
 public:
  TransformNode(TransformComponent* transform_component)
      : transform_component_(transform_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>();
    node_sig->AddInput<EntityRef>();
    node_sig->AddOutput<TransformDataRef>();
  }

  virtual void Initialize(NodeArguments* args) {
    auto entity = args->GetInput<EntityRef>(1);
    args->SetOutput(0, TransformDataRef(transform_component_, *entity));
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }

 private:
  TransformComponent* transform_component_;
};

// Returns the child at the given index.
class ChildNode : public BaseNode {
 public:
  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<TransformDataRef>();
    node_sig->AddInput<int>();
    node_sig->AddOutput<EntityRef>();
  }

  virtual void Initialize(NodeArguments* args) {
    auto transform_ref = args->GetInput<TransformDataRef>(0);
    if (transform_ref->entity()) {
      auto child_index = *args->GetInput<int>(1);
      TransformData* transform_data = transform_ref->GetComponentData();
      // Find the child at the given index.
      auto iter = transform_data->children.begin();
      while (child_index && iter != transform_data->children.end()) {
        ++iter;
        --child_index;
      }
      args->SetOutput(0, iter != transform_data->children.end() ? iter->owner
                                                                : EntityRef());
    } else {
      args->SetOutput(0, EntityRef());
    }
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }
};

// Returns the position of the entity in world space.
class WorldPositionNode : public BaseNode {
 public:
  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>();
    node_sig->AddInput<TransformDataRef>();
    node_sig->AddOutput<vec3>();
  }

  virtual void Initialize(NodeArguments* args) {
    if (args->IsInputDirty(0)) {
      auto transform_ref = args->GetInput<TransformDataRef>(1);
      vec3 position =
          transform_ref->component()->WorldPosition(transform_ref->entity());
      args->SetOutput(0, position);
    }
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }
};

void InitializeTransformModule(ModuleRegistry* module_registry,
                               TransformComponent* transform_component) {
  auto transform_ctor = [transform_component]() {
    return new TransformNode(transform_component);
  };
  TypeRegistry<TransformDataRef>::RegisterType("TransformData");
  Module* module = module_registry->RegisterModule("transform");
  module->RegisterNode<TransformNode>("transform", transform_ctor);
  module->RegisterNode<ChildNode>("child");
  module->RegisterNode<WorldPositionNode>("world_position");
}

}  // namespace module_library
}  // namespace fpl
