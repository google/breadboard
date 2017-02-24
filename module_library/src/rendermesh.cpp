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

#include "module_library/rendermesh.h"

#include "breadboard/base_node.h"
#include "breadboard/module_registry.h"

using breadboard::BaseNode;
using breadboard::ModuleRegistry;
using breadboard::Module;
using breadboard::NodeArguments;
using breadboard::NodeSignature;
using corgi::component_library::RenderMeshComponent;
using corgi::component_library::RenderMeshData;
using corgi::EntityRef;

namespace breadboard {
namespace module_library {

// Sets the visibility of RenderMesh recursively.
class SetVisibleNode : public BaseNode {
 public:
  enum { kInputEntity, kInputVisible };
  SetVisibleNode(RenderMeshComponent* render_mesh_component)
      : render_mesh_component_(render_mesh_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<EntityRef>(kInputEntity, "Entity");
    node_sig->AddInput<bool>(kInputVisible, "Visible");
  }

  virtual void Initialize(NodeArguments* args) {
    auto entity = args->GetInput<EntityRef>(kInputEntity);
    if (entity->IsValid()) {
      render_mesh_component_->SetVisibilityRecursively(
          *entity, *args->GetInput<bool>(kInputVisible));
    }
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }

 private:
  RenderMeshComponent* render_mesh_component_;
};

// Sets the tint color of RenderMesh.
class SetTintNode : public BaseNode {
 public:
  enum { kInputTrigger, kInputEntity, kInputTint };
  SetTintNode(RenderMeshComponent* render_mesh_component)
      : render_mesh_component_(render_mesh_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>(kInputTrigger, "Trigger");
    node_sig->AddInput<EntityRef>(kInputEntity, "Entity");
    node_sig->AddInput<mathfu::vec4>(kInputTint, "Tint");
  }

  virtual void Initialize(NodeArguments* args) {
    if (args->IsInputDirty(kInputTrigger)) {
      auto entity = args->GetInput<EntityRef>(kInputEntity);
      if (entity->IsValid()) {
        RenderMeshData* render_mesh_data =
            render_mesh_component_->GetComponentData(*entity);
        render_mesh_data->tint = *args->GetInput<mathfu::vec4>(kInputTint);
      }
    }
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }

 private:
  RenderMeshComponent* render_mesh_component_;
};

void InitializeRenderMeshModule(ModuleRegistry* module_registry,
                                RenderMeshComponent* render_mesh_component) {
  auto set_visible_ctor = [render_mesh_component]() {
    return new SetVisibleNode(render_mesh_component);
  };
  auto set_tint_ctor = [render_mesh_component]() {
    return new SetTintNode(render_mesh_component);
  };
  Module* module = module_registry->RegisterModule("rendermesh");
  module->RegisterNode<SetVisibleNode>("set_visible", set_visible_ctor);
  module->RegisterNode<SetTintNode>("set_tint", set_tint_ctor);
}

}  // namespace module_library
}  // namespace breadboard
