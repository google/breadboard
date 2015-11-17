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

#include "module_library/vec3.h"

#include "breadboard/base_node.h"
#include "breadboard/module_registry.h"
#include "mathfu/glsl_mappings.h"

using breadboard::BaseNode;
using breadboard::Module;
using breadboard::ModuleRegistry;
using breadboard::NodeArguments;
using breadboard::NodeSignature;
using breadboard::TypeRegistry;
using mathfu::vec3;

namespace breadboard {
namespace module_library {

// Creates a vector from 3 floats.
class Vec3Node : public BaseNode {
 public:
  virtual ~Vec3Node() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<float>();
    node_sig->AddInput<float>();
    node_sig->AddInput<float>();
    node_sig->AddOutput<vec3>();
  }

  virtual void Execute(NodeArguments* args) {
    auto x = args->GetInput<float>(0);
    auto y = args->GetInput<float>(1);
    auto z = args->GetInput<float>(2);
    args->SetOutput(0, vec3(*x, *y, *z));
  }
};

// Returns the individual elements of the given vector.
class ElementsNode : public BaseNode {
 public:
  virtual ~ElementsNode() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>();
    node_sig->AddOutput<float>();
    node_sig->AddOutput<float>();
    node_sig->AddOutput<float>();
  }

  virtual void Execute(NodeArguments* args) {
    auto vec = args->GetInput<vec3>(0);
    args->SetOutput(0, vec->x());
    args->SetOutput(1, vec->y());
    args->SetOutput(2, vec->z());
  }
};

// Adds the two given vectors.
class AddNode : public BaseNode {
 public:
  virtual ~AddNode() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>();
    node_sig->AddInput<vec3>();
    node_sig->AddOutput<vec3>();
  }

  virtual void Execute(NodeArguments* args) {
    auto vec_a = args->GetInput<vec3>(0);
    auto vec_b = args->GetInput<vec3>(1);
    args->SetOutput(0, *vec_a + *vec_b);
  }
};

// Subtracts the two given vectors.
class SubtractNode : public BaseNode {
 public:
  virtual ~SubtractNode() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>();
    node_sig->AddInput<vec3>();
    node_sig->AddOutput<vec3>();
  }

  virtual void Execute(NodeArguments* args) {
    auto vec_a = args->GetInput<vec3>(0);
    auto vec_b = args->GetInput<vec3>(1);
    args->SetOutput(0, *vec_a - *vec_b);
  }
};

class ScalarMultiplyNode : public BaseNode {
 public:
  virtual ~ScalarMultiplyNode() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>();
    node_sig->AddInput<float>();
    node_sig->AddOutput<vec3>();
  }

  virtual void Execute(NodeArguments* args) {
    auto vec = args->GetInput<vec3>(0);
    auto scalar = args->GetInput<float>(1);
    args->SetOutput(0, *vec * *scalar);
  }
};

class ScalarDivideNode : public BaseNode {
 public:
  virtual ~ScalarDivideNode() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>();
    node_sig->AddInput<float>();
    node_sig->AddOutput<vec3>();
  }

  virtual void Execute(NodeArguments* args) {
    auto vec = args->GetInput<vec3>(0);
    auto scalar = args->GetInput<float>(1);
    args->SetOutput(0, *vec / *scalar);
  }
};

class CrossProductNode : public BaseNode {
 public:
  virtual ~CrossProductNode() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>();
    node_sig->AddInput<vec3>();
    node_sig->AddOutput<vec3>();
  }

  virtual void Execute(NodeArguments* args) {
    auto vec_a = args->GetInput<vec3>(0);
    auto vec_b = args->GetInput<vec3>(1);
    args->SetOutput(0, vec3::CrossProduct(*vec_a, *vec_b));
  }
};

class DotProductNode : public BaseNode {
 public:
  virtual ~DotProductNode() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>();
    node_sig->AddInput<vec3>();
    node_sig->AddOutput<float>();
  }

  virtual void Execute(NodeArguments* args) {
    auto vec_a = args->GetInput<vec3>(0);
    auto vec_b = args->GetInput<vec3>(1);
    args->SetOutput(0, vec3::DotProduct(*vec_a, *vec_b));
  }
};

class LengthNode : public BaseNode {
 public:
  virtual ~LengthNode() {}

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>();
    node_sig->AddOutput<float>();
  }

  virtual void Execute(NodeArguments* args) {
    auto vec = args->GetInput<vec3>(0);
    args->SetOutput(0, vec->Length());
  }
};

void InitializeVec3Module(ModuleRegistry* module_registry) {
  TypeRegistry<vec3>::RegisterType("Vec3");
  Module* module = module_registry->RegisterModule("vec3");
  module->RegisterNode<Vec3Node>("vec3");
  module->RegisterNode<ElementsNode>("elements");
  module->RegisterNode<AddNode>("add");
  module->RegisterNode<SubtractNode>("subtract");
  module->RegisterNode<ScalarMultiplyNode>("scalar_multiply");
  module->RegisterNode<ScalarDivideNode>("scalar_divide");
  module->RegisterNode<CrossProductNode>("cross_product");
  module->RegisterNode<DotProductNode>("dot_product");
  module->RegisterNode<LengthNode>("length");
}

}  // namespace module_library
}  // namespace breadboard
