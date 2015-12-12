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

#include "module_library/physics.h"

#include <string>

#include "breadboard/base_node.h"
#include "breadboard/module_registry.h"
#include "corgi/entity_manager.h"
#include "corgi_component_library/physics.h"

using breadboard::BaseNode;
using breadboard::Module;
using breadboard::ModuleRegistry;
using breadboard::NodeArguments;
using breadboard::NodeSignature;
using corgi::component_library::CollisionData;
using corgi::component_library::GraphComponent;
using corgi::component_library::PhysicsComponent;
using corgi::component_library::kCollisionEventId;
using corgi::EntityRef;

namespace breadboard {
namespace module_library {

class OnCollisionNode : public BaseNode {
 public:
  enum { kInputEntity };
  enum { kOutputCollision };
  enum { kListenerOnCollision };

  OnCollisionNode(GraphComponent* graph_component)
      : graph_component_(graph_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<EntityRef>(kInputEntity, "Entity");
    node_sig->AddOutput<void>(kOutputCollision, "Collision");
    node_sig->AddListener(kCollisionEventId);
  }

  virtual void Initialize(NodeArguments* args) {
    auto entity = args->GetInput<EntityRef>(kInputEntity);
    args->BindBroadcaster(kListenerOnCollision,
                          graph_component_->GetCreateBroadcaster(*entity));
  }

  virtual void Execute(NodeArguments* args) {
    Initialize(args);
    if (args->IsListenerDirty(kListenerOnCollision)) {
      args->SetOutput(kOutputCollision);
    }
  }

 private:
  GraphComponent* graph_component_;
};

class CollisionDataNode : public BaseNode {
 public:
  enum { kInputTrigger };
  enum {
    kOutputEntityA,
    kOutputPositionA,
    kOutputTagA,
    kOutputEntityB,
    kOutputPositionB,
    kOutputTagB
  };

  CollisionDataNode(PhysicsComponent* physics_component)
      : physics_component_(physics_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    // Fetch the collision data when triggered
    node_sig->AddInput<void>(kInputTrigger);

    // One of the entities involved in the collision, the location of the
    // entity, and an arbitrary tag.
    node_sig->AddOutput<EntityRef>(kOutputEntityA, "Entity A");
    node_sig->AddOutput<mathfu::vec3>(kOutputPositionA, "Position A");
    node_sig->AddOutput<std::string>(kOutputTagA, "Tag A");

    // The other entity involved in the collision, the location of the entity,
    // and an arbitrary tag.
    node_sig->AddOutput<EntityRef>(kOutputEntityB, "Entity B");
    node_sig->AddOutput<mathfu::vec3>(kOutputPositionB, "Position B");
    node_sig->AddOutput<std::string>(kOutputTagB, "Tag B");
  }

  virtual void Initialize(NodeArguments* args) {
    CollisionData& collision_data = physics_component_->collision_data();
    args->SetOutput(kOutputEntityA, collision_data.this_entity);
    args->SetOutput(kOutputPositionA, collision_data.this_position);
    args->SetOutput(kOutputTagA, collision_data.this_tag);
    args->SetOutput(kOutputEntityB, collision_data.other_entity);
    args->SetOutput(kOutputPositionB, collision_data.other_position);
    args->SetOutput(kOutputTagB, collision_data.other_tag);
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }

 private:
  PhysicsComponent* physics_component_;
};

class VelocityNode : public BaseNode {
 public:
  enum { kInputTrigger, kInputEntity };
  enum { kOutputVelocity };

  VelocityNode(PhysicsComponent* physics_component)
      : physics_component_(physics_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>(kInputTrigger, "Trigger");
    node_sig->AddInput<EntityRef>(kInputEntity, "Entity");
    node_sig->AddOutput<mathfu::vec3>(kOutputVelocity, "Velocity");
  }

  virtual void Execute(NodeArguments* args) {
    if (args->IsInputDirty(kInputTrigger)) {
      auto entity = args->GetInput<EntityRef>(kInputEntity);
      auto physics_data = physics_component_->GetComponentData(*entity);
      args->SetOutput(kOutputVelocity, physics_data->Velocity());
    }
  }

 private:
  PhysicsComponent* physics_component_;
};

void InitializePhysicsModule(ModuleRegistry* module_registry,
                             PhysicsComponent* physics_component,
                             GraphComponent* graph_component) {
  Module* module = module_registry->RegisterModule("physics");
  auto on_collision_ctor = [graph_component]() {
    return new OnCollisionNode(graph_component);
  };
  auto collision_data_ctor = [physics_component]() {
    return new CollisionDataNode(physics_component);
  };
  auto velocity_ctor = [physics_component]() {
    return new VelocityNode(physics_component);
  };
  module->RegisterNode<OnCollisionNode>("on_collision", on_collision_ctor);
  module->RegisterNode<CollisionDataNode>("collision_data",
                                          collision_data_ctor);
  module->RegisterNode<VelocityNode>("velocity", velocity_ctor);
}

}  // namespace module_library
}  // namespace breadboard
