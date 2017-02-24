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

#include "breadboard/modules/string.h"

#include <string>
#include <sstream>

#include "breadboard/base_node.h"
#include "breadboard/module_registry.h"

namespace breadboard {

// Compares two strings.
class EqualsNode : public BaseNode {
 public:
  enum { kInputA, kInputB };
  enum { kOutputResult };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<std::string>(kInputA, "A");
    node_sig->AddInput<std::string>(kInputB, "B");
    node_sig->AddOutput<bool>(kOutputResult, "Result");
  }

  virtual void Initialize(NodeArguments* args) {
    auto str_a = args->GetInput<std::string>(kInputA);
    auto str_b = args->GetInput<std::string>(kInputB);
    args->SetOutput(kOutputResult, *str_a == *str_b);
  }

  virtual void Execute(NodeArguments* args) { Initialize(args); }
};

// Converts the given int to a string.
class IntToStringNode : public BaseNode {
 public:
  enum { kInputInt };
  enum { kOutputString };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<int>(kInputInt, "Int");
    node_sig->AddOutput<std::string>(kOutputString, "String");
  }

  virtual void Execute(NodeArguments* args) {
    auto i = args->GetInput<int>(kInputInt);
    std::stringstream stream;
    stream << *i;
    args->SetOutput(kOutputString, stream.str());
  }
};

// Converts the given float to a string.
class FloatToStringNode : public BaseNode {
 public:
  enum { kInputFloat };
  enum { kOutputString };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<float>(kInputFloat, "Float");
    node_sig->AddOutput<std::string>(kOutputString, "String");
  }

  virtual void Execute(NodeArguments* args) {
    auto f = args->GetInput<float>(kInputFloat);
    std::stringstream stream;
    stream << *f;
    args->SetOutput(kOutputString, stream.str());
  }
};

// Concatenates the given strings.
class ConcatNode : public BaseNode {
 public:
  enum { kInputA, kInputB };
  enum { kOutputResult };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<std::string>(kInputA, "A");
    node_sig->AddInput<std::string>(kInputB, "B");
    node_sig->AddOutput<std::string>(kOutputResult, "Result");
  }

  virtual void Execute(NodeArguments* args) {
    auto str_a = args->GetInput<std::string>(kInputA);
    auto str_b = args->GetInput<std::string>(kInputB);
    args->SetOutput(kOutputResult, *str_a + *str_b);
  }
};

void InitializeStringModule(ModuleRegistry* module_registry) {
  Module* module = module_registry->RegisterModule("string");
  module->RegisterNode<EqualsNode>("equals");
  module->RegisterNode<IntToStringNode>("int_to_string");
  module->RegisterNode<FloatToStringNode>("float_to_string");
  module->RegisterNode<ConcatNode>("concat");
}

}  // namespace breadboard
