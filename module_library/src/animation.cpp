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

#include "module_library/animation.h"

#include <string>

#include "breadboard/base_node.h"
#include "breadboard/module_registry.h"
#include "component_library/animation.h"
#include "component_library/meta.h"
#include "component_library/transform.h"
#include "entity/entity_manager.h"

namespace breadboard {
namespace module_library {

using breadboard::BaseNode;
using breadboard::ModuleRegistry;
using breadboard::Module;
using breadboard::NodeArguments;
using breadboard::NodeSignature;
using corgi::EntityRef;
using corgi::component_library::AnimationComponent;
using corgi::component_library::GraphComponent;
using corgi::component_library::kAnimationCompleteEventId;
using corgi::component_library::TransformComponent;

static inline EntityRef ChildAnimEntity(TransformComponent* transform_component,
                                        const EntityRef& entity) {
  EntityRef anim_entity = transform_component->ChildWithComponent(
      entity, AnimationComponent::GetComponentId());
  assert(anim_entity.IsValid());
  return anim_entity;
}

// Executes when the animation on the given entity is complete.
class AnimationCompleteNode : public BaseNode {
 public:
  explicit AnimationCompleteNode(GraphComponent* graph_component)
      : graph_component_(graph_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<EntityRef>();
    node_sig->AddOutput<void>();
    node_sig->AddListener(kAnimationCompleteEventId);
  }

  virtual void Initialize(NodeArguments* args) {
    EntityRef entity = *args->GetInput<EntityRef>(0);
    if (entity) {
      args->BindBroadcaster(0, graph_component_->GetCreateBroadcaster(entity));
    }
  }

  virtual void Execute(NodeArguments* args) {
    Initialize(args);
    if (args->IsListenerDirty(0)) {
      args->SetOutput(0);
    }
  }

 private:
  GraphComponent* graph_component_;
};

// Starts the requested animation on the requested entity.
class PlayAnimationNode : public BaseNode {
 public:
  PlayAnimationNode(AnimationComponent* anim_component,
                    TransformComponent* transform_component)
      : anim_component_(anim_component),
        transform_component_(transform_component) {}

  static void OnRegister(NodeSignature* node_sig) {
    // Void to trigger the animation,
    // the entity to be animated,
    // and the index into the AnimTable for this entity.
    node_sig->AddInput<void>();
    node_sig->AddInput<EntityRef>();
    node_sig->AddInput<int>();
    node_sig->AddOutput<void>();
  }

  virtual void Execute(NodeArguments* args) {
    if (args->IsInputDirty(0)) {
      EntityRef entity = *args->GetInput<EntityRef>(1);
      EntityRef anim_entity = ChildAnimEntity(transform_component_, entity);
      const int current_anim_idx = anim_component_->LastAnimIdx(anim_entity);
      int anim_idx = *args->GetInput<int>(2);
      if (current_anim_idx != anim_idx) {
        anim_component_->AnimateFromTable(anim_entity, anim_idx);
      }
      args->SetOutput(0);
    }
  }

 private:
  AnimationComponent* anim_component_;
  TransformComponent* transform_component_;
};

// Returns the index of the last animation played.
class AnimationIndexNode : public breadboard::BaseNode {
 public:
  AnimationIndexNode(AnimationComponent* anim_component,
                     TransformComponent* transform_component)
      : anim_component_(anim_component),
        transform_component_(transform_component) {}

  static void OnRegister(breadboard::NodeSignature* node_sig) {
    // Void to trigger the animation,
    // the entity to be animated,
    // and the index into the AnimTable for this entity.
    node_sig->AddInput<void>();
    node_sig->AddInput<EntityRef>();
    node_sig->AddOutput<int>();
  }

  virtual void Execute(breadboard::NodeArguments* args) {
    if (args->IsInputDirty(0)) {
      EntityRef entity = *args->GetInput<EntityRef>(1);
      EntityRef anim_entity = ChildAnimEntity(transform_component_, entity);
      const int anim_idx = anim_component_->LastAnimIdx(anim_entity);
      args->SetOutput(0, anim_idx);
    }
  }

 private:
  AnimationComponent* anim_component_;
  TransformComponent* transform_component_;
};

void InitializeAnimationModule(ModuleRegistry* module_registry,
                               GraphComponent* graph_component,
                               AnimationComponent* anim_component,
                               TransformComponent* transform_component) {
  auto animation_complete_ctor = [graph_component]() {
    return new AnimationCompleteNode(graph_component);
  };
  auto play_animation_ctor = [anim_component, transform_component]() {
    return new PlayAnimationNode(anim_component, transform_component);
  };
  auto animation_index_ctor = [anim_component, transform_component]() {
    return new AnimationIndexNode(anim_component, transform_component);
  };

  Module* module = module_registry->RegisterModule("animation");
  module->RegisterNode<AnimationCompleteNode>("animation_complete",
                                              animation_complete_ctor);
  module->RegisterNode<PlayAnimationNode>("play_animation",
                                          play_animation_ctor);
  module->RegisterNode<AnimationIndexNode>("animation_index",
                                           animation_index_ctor);
}

}  // namespace module_library
}  // namespace breadboard
