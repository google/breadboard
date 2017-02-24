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
  enum { kInputPlay, kInputSoundHandle, kInputLocation, kInputGain };
  enum { kOutputChannel };

  PlaySoundNode(AudioEngine* audio_engine) : audio_engine_(audio_engine) {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>(kInputPlay, "Play");
    node_sig->AddInput<SoundHandle>(kInputSoundHandle, "Sound");
    node_sig->AddInput<vec3>(kInputLocation, "Location");
    node_sig->AddInput<float>(kInputGain, "Gain");
    node_sig->AddOutput<Channel>(kOutputChannel, "Channel");
  }

  virtual void Execute(NodeArguments* args) {
    if (args->IsInputDirty(kInputPlay)) {
      auto handle = args->GetInput<SoundHandle>(kInputSoundHandle);
      auto location = args->GetInput<vec3>(kInputLocation);
      auto gain = args->GetInput<float>(kInputGain);
      Channel channel = audio_engine_->PlaySound(*handle, *location, *gain);
      args->SetOutput(kOutputChannel, channel);
    }
  }

 private:
  AudioEngine* audio_engine_;
};

// Checks if a given audio channel is playing.
class PlayingNode : public BaseNode {
 public:
  enum { kInputChannel };
  enum { kOutputChannel, kOutputPlaying };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<Channel>(kInputChannel, "Channel");
    node_sig->AddOutput<Channel>(kOutputChannel, "Channel");
    node_sig->AddOutput<bool>(kOutputPlaying, "Result");
  }

  virtual void Execute(NodeArguments* args) {
    auto channel = args->GetInput<Channel>(kInputChannel);
    args->SetOutput(kOutputChannel, *channel);
    args->SetOutput(kOutputPlaying, channel->Playing());
  }
};

// Stops the given audio channel.
class StopNode : public BaseNode {
 public:
  enum { kInputChannel };
  enum { kOutputChannel };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<Channel>(kInputChannel, "Channel");
    node_sig->AddOutput<Channel>(kOutputChannel, "Channel");
  }

  virtual void Execute(NodeArguments* args) {
    auto channel = args->GetInput<Channel>(kInputChannel);
    channel->Stop();
    args->SetOutput(kOutputChannel, *channel);
  }
};

// Set the gain on the given audio channel.
class SetGainNode : public BaseNode {
 public:
  enum { kInputChannel, kInputGain };
  enum { kOutputChannel };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<Channel>(kInputChannel, "Channel");
    node_sig->AddInput<float>(kInputGain, "Gain");
    node_sig->AddOutput<Channel>(kOutputChannel, "Channel");
  }

  virtual void Execute(NodeArguments* args) {
    auto channel = args->GetInput<Channel>(kInputChannel);
    auto gain = args->GetInput<float>(kInputGain);
    channel->SetGain(*gain);
    args->SetOutput(kOutputChannel, *channel);
  }
};

// Returns the gain of the given audio channel.
class GainNode : public BaseNode {
 public:
  enum { kInputChannel };
  enum { kOutputChannel, kOutputGain };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<Channel>(kInputChannel, "Channel");
    node_sig->AddOutput<Channel>(kOutputChannel, "Channel");
    node_sig->AddOutput<float>(kOutputGain, "Gain");
  }

  virtual void Execute(NodeArguments* args) {
    auto channel = args->GetInput<Channel>(kInputChannel);
    args->SetOutput(kOutputChannel, *channel);
    args->SetOutput(kOutputGain, channel->Gain());
  }
};

// Sets the location of the given sound channel.
class SetLocationNode : public BaseNode {
 public:
  enum { kInputChannel, kInputLocation };
  enum { kOutputChannel };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<Channel>(kInputChannel);
    node_sig->AddInput<vec3>(kInputLocation);
    node_sig->AddOutput<Channel>(kOutputChannel);
  }

  virtual void Execute(NodeArguments* args) {
    auto channel = args->GetInput<Channel>(kInputChannel);
    auto location = args->GetInput<vec3>(kInputChannel);
    channel->SetLocation(*location);
    args->SetOutput(kOutputChannel, *channel);
  }
};

// Returns the location of the given audio channel.
class LocationNode : public BaseNode {
 public:
  enum { kInputChannel };
  enum { kOutputChannel, kOutputLocation };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<Channel>(kInputChannel, "Channel");
    node_sig->AddOutput<Channel>(kOutputChannel, "Channel");
    node_sig->AddOutput<vec3>(kOutputLocation, "Location");
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
