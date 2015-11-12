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

#include "module_library/entity.h"

#include "breadboard/base_node.h"
#include "breadboard/module_registry.h"
#include "component_library/meta.h"
#include "entity/entity_manager.h"
#include "mathfu/glsl_mappings.h"

using breadboard::BaseNode;
using breadboard::ModuleRegistry;
using breadboard::Module;
using breadboard::NodeArguments;
using breadboard::NodeSignature;
using breadboard::TypeRegistry;
using corgi::component_library::GraphComponent;
using corgi::component_library::MetaComponent;
using corgi::EntityManager;
using corgi::EntityRef;

namespace breadboard {
namespace module_library {

// Given an input string, return the named entity.
class EntityNode : public BaseNode {
 public:
  EntityNode(MetaComponent* meta_component) : meta_component_(meta_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>();
    node_sig->AddInput<std::string>();
    node_sig->AddOutput<EntityRef>();
  }

  virtual void Execute(NodeArguments* args) {
    auto entity_id = args->GetInput<std::string>(1);
    EntityRef entity =
        meta_component_->GetEntityFromDictionary(entity_id->c_str());
    assert(entity.IsValid());
    args->SetOutput(0, entity);
  }

 private:
  MetaComponent* meta_component_;
};

// Return the entity that owns this graph.
class GraphEntityNode : public BaseNode {
 public:
  GraphEntityNode(GraphComponent* graph_component)
      : graph_component_(graph_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddOutput<EntityRef>();
  }

  virtual void Initialize(NodeArguments* args) {
    args->SetOutput(0, graph_component_->graph_entity());
  }

 private:
  GraphComponent* graph_component_;
};

// Delete the given entity.
class DeleteEntityNode : public BaseNode {
 public:
  DeleteEntityNode(::EntityManager* entity_manager)
      : entity_manager_(entity_manager) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>();
    node_sig->AddInput<EntityRef>();
  }

  virtual void Execute(NodeArguments* args) {
    if (args->IsInputDirty(0)) {
      auto entity_ref = args->GetInput<EntityRef>(1);
      entity_manager_->DeleteEntity(*entity_ref);
    }
  }

 private:
  EntityManager* entity_manager_;
};

void InitializeEntityModule(ModuleRegistry* module_registry,
                            EntityManager* entity_manager,
                            MetaComponent* meta_component,
                            GraphComponent* graph_component) {
  auto entity_ctor = [meta_component]() {
    return new EntityNode(meta_component);
  };
  auto graph_entity_ctor = [graph_component]() {
    return new GraphEntityNode(graph_component);
  };
  auto delete_entity_ctor = [entity_manager]() {
    return new DeleteEntityNode(entity_manager);
  };
  TypeRegistry<EntityRef>::RegisterType("Entity");
  Module* module = module_registry->RegisterModule("entity");
  module->RegisterNode<EntityNode>("entity", entity_ctor);
  module->RegisterNode<GraphEntityNode>("graph_entity", graph_entity_ctor);
  module->RegisterNode<DeleteEntityNode>("delete_entity", delete_entity_ctor);
}

}  // namespace module_library
}  // namespace breadboard
