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
  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<bool>();
    node_sig->AddOutput<void>();  // Fires a pulse when input is true.
    node_sig->AddOutput<void>();  // Fires a pulse when input is false.
  }

  virtual void Initialize(NodeArguments* args) {
    auto value = args->GetInput<bool>(0);
    args->SetOutput(*value ? 0 : 1);
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }
};

// Logical Not.
class NotNode : public BaseNode {
 public:
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

void InitializeLogicModule(ModuleRegistry* module_registry) {
  Module* module = module_registry->RegisterModule("logic");
  module->RegisterNode<IfNode>("if");
  module->RegisterNode<AndNode>("and");
  module->RegisterNode<OrNode>("or");
  module->RegisterNode<XorNode>("xor");
  module->RegisterNode<NotNode>("not");
}

}  // namespace breadboard
