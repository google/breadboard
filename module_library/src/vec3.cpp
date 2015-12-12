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
  enum { kInputX, kInputY, kInputZ };
  enum { kOutputVec };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<float>(kInputX, "X");
    node_sig->AddInput<float>(kInputY, "Y");
    node_sig->AddInput<float>(kInputZ, "Z");
    node_sig->AddOutput<vec3>(kOutputVec, "Vector");
  }

  virtual void Execute(NodeArguments* args) {
    auto x = args->GetInput<float>(kInputX);
    auto y = args->GetInput<float>(kInputY);
    auto z = args->GetInput<float>(kInputZ);
    args->SetOutput(kOutputVec, vec3(*x, *y, *z));
  }
};

// Returns the individual elements of the given vector.
class ElementsNode : public BaseNode {
 public:
  enum { kInputVector };
  enum { kOutputX, kOutputY, kOutputZ };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>(kInputVector, "Vector");
    node_sig->AddOutput<float>(kOutputX, "X");
    node_sig->AddOutput<float>(kOutputY, "Y");
    node_sig->AddOutput<float>(kOutputZ, "Z");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec = args->GetInput<vec3>(kInputVector);
    args->SetOutput(kOutputX, vec->x());
    args->SetOutput(kOutputY, vec->y());
    args->SetOutput(kOutputZ, vec->z());
  }
};

// Adds the two given vectors.
class AddNode : public BaseNode {
 public:
  enum { kInputA, kInputB };
  enum { kOutputSum };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>(kInputA, "A");
    node_sig->AddInput<vec3>(kInputB, "B");
    node_sig->AddOutput<vec3>(kOutputSum, "Sum");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec_a = args->GetInput<vec3>(kInputA);
    auto vec_b = args->GetInput<vec3>(kInputB);
    args->SetOutput(kOutputSum, *vec_a + *vec_b);
  }
};

// Subtracts the two given vectors.
class SubtractNode : public BaseNode {
 public:
  enum { kInputA, kInputB };
  enum { kOutputDifference };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>(kInputA, "A");
    node_sig->AddInput<vec3>(kInputB, "B");
    node_sig->AddOutput<vec3>(kOutputDifference, "Difference");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec_a = args->GetInput<vec3>(kInputA);
    auto vec_b = args->GetInput<vec3>(kInputB);
    args->SetOutput(kOutputDifference, *vec_a - *vec_b);
  }
};

class ScalarMultiplyNode : public BaseNode {
 public:
  enum { kInputVector, kInputScalar };
  enum { kOutputProduct };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>(kInputVector, "Vector");
    node_sig->AddInput<float>(kInputScalar, "Scalar");
    node_sig->AddOutput<vec3>(kOutputProduct, "Product");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec = args->GetInput<vec3>(kInputVector);
    auto scalar = args->GetInput<float>(kInputScalar);
    args->SetOutput(kOutputProduct, *vec * *scalar);
  }
};

class ScalarDivideNode : public BaseNode {
 public:
  enum { kInputVector, kInputScalar };
  enum { kOutputQuotient };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>(kInputVector, "Vector");
    node_sig->AddInput<float>(kInputScalar, "Scalar");
    node_sig->AddOutput<vec3>(kOutputQuotient, "Quotient");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec = args->GetInput<vec3>(kInputVector);
    auto scalar = args->GetInput<float>(kInputScalar);
    args->SetOutput(kOutputQuotient, *vec / *scalar);
  }
};

class CrossProductNode : public BaseNode {
 public:
  enum { kInputA, kInputB };
  enum { kOutputCrossProduct };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>(kInputA, "A");
    node_sig->AddInput<vec3>(kInputB, "B");
    node_sig->AddOutput<vec3>(kOutputCrossProduct, "Cross Product");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec_a = args->GetInput<vec3>(kInputA);
    auto vec_b = args->GetInput<vec3>(kInputB);
    args->SetOutput(kOutputCrossProduct, vec3::CrossProduct(*vec_a, *vec_b));
  }
};

class DotProductNode : public BaseNode {
 public:
  enum { kInputA, kInputB };
  enum { kOutputDotProduct };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>(kInputA, "A");
    node_sig->AddInput<vec3>(kInputB, "B");
    node_sig->AddOutput<float>(kOutputDotProduct, "Dot Product");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec_a = args->GetInput<vec3>(kInputA);
    auto vec_b = args->GetInput<vec3>(kInputB);
    args->SetOutput(kOutputDotProduct, vec3::DotProduct(*vec_a, *vec_b));
  }
};

class LengthNode : public BaseNode {
 public:
  enum { kInputVector };
  enum { kOutputLength };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec3>(kInputVector, "Vector");
    node_sig->AddOutput<float>(kOutputLength, "Length");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec = args->GetInput<vec3>(kInputVector);
    args->SetOutput(kOutputLength, vec->Length());
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
