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

#include "breadboard/modules/logic.h"

#include "breadboard/base_node.h"
#include "breadboard/module_registry.h"

namespace breadboard {

// clang-format off
#define LOGICAL_NODE(name, op)                        \
  class name : public BaseNode {                      \
   public:                                            \
    virtual ~name() {}                                \
                                                      \
    static void OnRegister(NodeSignature* node_sig) { \
      node_sig->AddInput<bool>();                     \
      node_sig->AddInput<bool>();                     \
      node_sig->AddOutput<bool>();                    \
    }                                                 \
                                                      \
    virtual void Initialize(NodeArguments* args) {    \
      auto a = args->GetInput<bool>(0);               \
      auto b = args->GetInput<bool>(1);               \
      bool result = *a op *b;                         \
      args->SetOutput(0, result);                     \
    }                                                 \
                                                      \
    virtual void Execute(NodeArguments* args) {       \
      Initialize(args);                               \
    }                                                 \
  }

LOGICAL_NODE(AndNode, &&);
LOGICAL_NODE(OrNode, ||);
LOGICAL_NODE(XorNode, ^);
// clang-format on

// Convert a boolean value to a pulse. The first edge is triggered when the
// result evaluates true, the second is triggered when the result evaluates
// false.
class IfNode : public BaseNode {
 public:
  virtual ~IfNode() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<bool>();
    node_sig->AddOutput<void>();  // Fires a pulse when input is true.
    node_sig->AddOutput<void>();  // Fires a pulse when input is false.
  }

  virtual void Execute(NodeArguments* args) {
    auto value = args->GetInput<bool>(0);
    args->SetOutput(*value ? 0 : 1);
  }
};

// Convert a boolean value to a pulse. The result is only propagated when the
// first argument is pulsed. The first edge is triggered when the result
// evaluates true, the second is triggered when the result evaluates false.
class IfGateNode : public BaseNode {
 public:
  virtual ~IfGateNode() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>();
    node_sig->AddInput<bool>();
    node_sig->AddOutput<void>();  // Fires a pulse when input is true.
    node_sig->AddOutput<void>();  // Fires a pulse when input is false.
  }

  virtual void Execute(NodeArguments* args) {
    if (args->IsInputDirty(0)) {
      auto value = args->GetInput<bool>(1);
      args->SetOutput(*value ? 0 : 1);
    }
  }
};

// Logical Not.
class NotNode : public BaseNode {
 public:
  virtual ~NotNode() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<bool>();
    node_sig->AddOutput<bool>();
  }

  virtual void Initialize(NodeArguments* args) {
    auto value = args->GetInput<bool>(0);
    bool result = !*value;
    args->SetOutput(0, result);
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }
};

// Stay Latch, to store boolean results.
class StayLatchNode : public BaseNode {
 public:
  virtual ~StayLatchNode() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>();  // Set to True
    node_sig->AddInput<void>();  // Set to False
    node_sig->AddOutput<bool>();
  }

  virtual void Initialize(NodeArguments* args) {
    args->SetOutput(0, false);
  }

  virtual void Execute(NodeArguments* args) {
    if (args->IsInputDirty(0)) {
      args->SetOutput(0, true);
    } else if (args->IsInputDirty(1)) {
      args->SetOutput(0, false);
    }
  }
};

void InitializeLogicModule(ModuleRegistry* module_registry) {
  Module* module = module_registry->RegisterModule("logic");
  module->RegisterNode<IfNode>("if");
  module->RegisterNode<IfGateNode>("if_gate");
  module->RegisterNode<AndNode>("and");
  module->RegisterNode<OrNode>("or");
  module->RegisterNode<XorNode>("xor");
  module->RegisterNode<NotNode>("not");
  module->RegisterNode<StayLatchNode>("stay_latch");
}

}  // namespace breadboard
