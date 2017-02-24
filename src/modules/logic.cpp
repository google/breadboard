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
    enum { kInputA, kInputB };                        \
    enum { kOutputResult };                           \
                                                      \
    static void OnRegister(NodeSignature* node_sig) { \
      node_sig->AddInput<bool>(kInputA);              \
      node_sig->AddInput<bool>(kInputB);              \
      node_sig->AddOutput<bool>(kOutputResult);       \
    }                                                 \
                                                      \
    virtual void Initialize(NodeArguments* args) {    \
      auto a = args->GetInput<bool>(kInputA);         \
      auto b = args->GetInput<bool>(kInputB);         \
      bool result = *a op *b;                         \
      args->SetOutput(kOutputResult, result);         \
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
  enum { kInputCondition };
  enum { kOutputTrue, kOutputFalse };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<bool>(kInputCondition, "Condition");
    node_sig->AddOutput<void>(kOutputTrue, "True");
    node_sig->AddOutput<void>(kOutputFalse, "False");
  }

  virtual void Execute(NodeArguments* args) {
    auto value = args->GetInput<bool>(kInputCondition);
    args->SetOutput(*value ? kOutputTrue : kOutputFalse);
  }
};

// Convert a boolean value to a pulse. The result is only propagated when the
// first argument is pulsed. The first edge is triggered when the result
// evaluates true, the second is triggered when the result evaluates false.
class IfGateNode : public BaseNode {
 public:
  enum { kInputTrigger, kInputCondition };
  enum { kOutputTrue, kOutputFalse };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>(kInputTrigger, "Trigger");
    node_sig->AddInput<bool>(kInputCondition, "Condition");
    node_sig->AddOutput<void>(kOutputTrue, "True");
    node_sig->AddOutput<void>(kOutputFalse, "False");
  }

  virtual void Execute(NodeArguments* args) {
    if (args->IsInputDirty(kInputTrigger)) {
      auto value = args->GetInput<bool>(kInputCondition);
      args->SetOutput(*value ? kOutputTrue : kOutputFalse);
    }
  }
};

// Logical Not.
class NotNode : public BaseNode {
 public:
  enum { kInput };
  enum { kOutput };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<bool>(kInput, "In");
    node_sig->AddOutput<bool>(kOutput, "Out");
  }

  virtual void Initialize(NodeArguments* args) {
    auto value = args->GetInput<bool>(kInput);
    bool result = !*value;
    args->SetOutput(kOutput, result);
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }
};

// Stay Latch, to store boolean results.
class StayLatchNode : public BaseNode {
 public:
  enum { kInputTrue, kInputFalse };
  enum { kOutputBoolean };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<void>(kInputTrue, "True");
    node_sig->AddInput<void>(kInputFalse, "False");
    node_sig->AddOutput<bool>(kOutputBoolean, "Boolean");
  }

  virtual void Initialize(NodeArguments* args) {
    args->SetOutput(kOutputBoolean, false);
  }

  virtual void Execute(NodeArguments* args) {
    if (args->IsInputDirty(kInputTrue)) {
      args->SetOutput(kOutputBoolean, true);
    } else if (args->IsInputDirty(kInputFalse)) {
      args->SetOutput(kOutputBoolean, false);
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
