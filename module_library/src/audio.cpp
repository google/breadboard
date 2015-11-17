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

#include "module_library/audio.h"

#include "breadboard/base_node.h"
#include "breadboard/module_registry.h"
#include "breadboard/type_registry.h"
#include "mathfu/glsl_mappings.h"
#include "pindrop/pindrop.h"

using breadboard::BaseNode;
using breadboard::ModuleRegistry;
using breadboard::Module;
using breadboard::NodeArguments;
using breadboard::NodeSignature;
using breadboard::TypeRegistry;
using mathfu::vec3;
using pindrop::AudioEngine;
using pindrop::Channel;
using pindrop::SoundHandle;

namespace breadboard {
namespace module_library {

// Plays the given sound. Also takes a gain and location arguments.
// Returns the channel the sound is playing on as an output.
class PlaySoundNode : public BaseNode {
 public:
  PlaySoundNode(AudioEngine* audio_engine) : audio_engine_(audio_engine) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>();         // Play the sound.
    node_sig->AddInput<SoundHandle>();  // The sound to play.
    node_sig->AddInput<vec3>();         // Where to play the sound.
    node_sig->AddInput<float>();        // The gain of the sound.
    node_sig->AddOutput<Channel>();     // The sound's channel.
  }

  virtual void Execute(NodeArguments* args) {
    if (args->IsInputDirty(0)) {
      auto handle = args->GetInput<SoundHandle>(1);
      auto location = args->GetInput<vec3>(2);
      auto gain = args->GetInput<float>(3);
      Channel channel = audio_engine_->PlaySound(*handle, *location, *gain);
      args->SetOutput(0, channel);
    }
  }

 private:
  AudioEngine* audio_engine_;
};

// Checks if a given audio channel is playing.
class PlayingNode : public BaseNode {
 public:
  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<Channel>();
    node_sig->AddOutput<Channel>();
    node_sig->AddOutput<bool>();
  }

  virtual void Execute(NodeArguments* args) {
    auto channel = args->GetInput<Channel>(0);
    args->SetOutput(0, *channel);
    args->SetOutput(1, channel->Playing());
  }
};

// Stops the given audio channel.
class StopNode : public BaseNode {
 public:
  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<Channel>();
    node_sig->AddOutput<Channel>();
  }

  virtual void Execute(NodeArguments* args) {
    auto channel = args->GetInput<Channel>(0);
    channel->Stop();
    args->SetOutput(0, *channel);
  }
};

// Set the gain on the given audio channel.
class SetGainNode : public BaseNode {
 public:
  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<Channel>();
    node_sig->AddInput<float>();
    node_sig->AddOutput<Channel>();
  }

  virtual void Execute(NodeArguments* args) {
    auto channel = args->GetInput<Channel>(0);
    auto gain = args->GetInput<float>(0);
    channel->SetGain(*gain);
    args->SetOutput(0, *channel);
  }
};

// Returns the gain of the given audio channel.
class GainNode : public BaseNode {
 public:
  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<Channel>();
    node_sig->AddOutput<Channel>();
    node_sig->AddOutput<float>();
  }

  virtual void Execute(NodeArguments* args) {
    auto channel = args->GetInput<Channel>(0);
    args->SetOutput(0, *channel);
    args->SetOutput(1, channel->Gain());
  }
};

// Sets the location of the given sound channel.
class SetLocationNode : public BaseNode {
 public:
  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<Channel>();
    node_sig->AddInput<vec3>();
    node_sig->AddOutput<Channel>();
  }

  virtual void Execute(NodeArguments* args) {
    auto channel = args->GetInput<Channel>(0);
    auto location = args->GetInput<vec3>(1);
    channel->SetLocation(*location);
    args->SetOutput(0, *channel);
  }
};

// Returns the location of the given audio channel.
class LocationNode : public BaseNode {
 public:
  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<Channel>();
    node_sig->AddOutput<Channel>();
    node_sig->AddOutput<vec3>();
  }

  virtual void Execute(NodeArguments* args) {
    auto channel = args->GetInput<Channel>(0);
    args->SetOutput(0, *channel);
    args->SetOutput(1, channel->Location());
  }
};

void InitializeAudioModule(ModuleRegistry* module_registry,
                           AudioEngine* audio_engine) {
  auto play_sound_ctor = [audio_engine]() {
    return new PlaySoundNode(audio_engine);
  };
  TypeRegistry<Channel>::RegisterType("Channel");
  TypeRegistry<SoundHandle>::RegisterType("SoundHandle");
  Module* module = module_registry->RegisterModule("audio");
  module->RegisterNode<PlaySoundNode>("play_sound", play_sound_ctor);
  module->RegisterNode<PlayingNode>("playing");
  module->RegisterNode<StopNode>("stop");
  module->RegisterNode<SetGainNode>("set_gain");
  module->RegisterNode<GainNode>("gain");
  module->RegisterNode<SetLocationNode>("set_location");
  module->RegisterNode<LocationNode>("location");
}

}  // namespace module_library
}  // namespace breadboard
