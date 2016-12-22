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

#include "module_library/vec.h"

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
using mathfu::vec4;

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
class ElementsVec3Node : public BaseNode {
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
    args->SetOutput(kOutputX, vec->x);
    args->SetOutput(kOutputY, vec->y);
    args->SetOutput(kOutputZ, vec->z);
  }
};

// Creates a vector from 4 floats.
class Vec4Node : public BaseNode {
 public:
  enum { kInputX, kInputY, kInputZ, kInputW };
  enum { kOutputVec };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<float>(kInputX, "X");
    node_sig->AddInput<float>(kInputY, "Y");
    node_sig->AddInput<float>(kInputZ, "Z");
    node_sig->AddInput<float>(kInputW, "W");
    node_sig->AddOutput<vec3>(kOutputVec, "Vector");
  }

  virtual void Execute(NodeArguments* args) {
    auto x = args->GetInput<float>(kInputX);
    auto y = args->GetInput<float>(kInputY);
    auto z = args->GetInput<float>(kInputZ);
    auto w = args->GetInput<float>(kInputW);
    args->SetOutput(kOutputVec, vec4(*x, *y, *z, *w));
  }
};

// Returns the individual elements of the given vector.
class ElementsVec4Node : public BaseNode {
 public:
  enum { kInputVector };
  enum { kOutputX, kOutputY, kOutputZ, kOutputW };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<vec4>(kInputVector, "Vector");
    node_sig->AddOutput<float>(kOutputX, "X");
    node_sig->AddOutput<float>(kOutputY, "Y");
    node_sig->AddOutput<float>(kOutputZ, "Z");
    node_sig->AddOutput<float>(kOutputW, "W");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec = args->GetInput<vec4>(kInputVector);
    args->SetOutput(kOutputX, vec->x);
    args->SetOutput(kOutputY, vec->y);
    args->SetOutput(kOutputZ, vec->z);
    args->SetOutput(kOutputW, vec->w);
  }
};

// Adds the two given vectors.
template <typename T>
class AddNode : public BaseNode {
 public:
  enum { kInputA, kInputB };
  enum { kOutputSum };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<T>(kInputA, "A");
    node_sig->AddInput<T>(kInputB, "B");
    node_sig->AddOutput<T>(kOutputSum, "Sum");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec_a = args->GetInput<T>(kInputA);
    auto vec_b = args->GetInput<T>(kInputB);
    args->SetOutput(kOutputSum, *vec_a + *vec_b);
  }
};

// Subtracts the two given vectors.
template <typename T>
class SubtractNode : public BaseNode {
 public:
  enum { kInputA, kInputB };
  enum { kOutputDifference };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<T>(kInputA, "A");
    node_sig->AddInput<T>(kInputB, "B");
    node_sig->AddOutput<T>(kOutputDifference, "Difference");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec_a = args->GetInput<T>(kInputA);
    auto vec_b = args->GetInput<T>(kInputB);
    args->SetOutput(kOutputDifference, *vec_a - *vec_b);
  }
};

template <typename T>
class ScalarMultiplyNode : public BaseNode {
 public:
  enum { kInputVector, kInputScalar };
  enum { kOutputProduct };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<T>(kInputVector, "Vector");
    node_sig->AddInput<float>(kInputScalar, "Scalar");
    node_sig->AddOutput<T>(kOutputProduct, "Product");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec = args->GetInput<T>(kInputVector);
    auto scalar = args->GetInput<float>(kInputScalar);
    args->SetOutput(kOutputProduct, *vec * *scalar);
  }
};

template <typename T>
class ScalarDivideNode : public BaseNode {
 public:
  enum { kInputVector, kInputScalar };
  enum { kOutputQuotient };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<T>(kInputVector, "Vector");
    node_sig->AddInput<float>(kInputScalar, "Scalar");
    node_sig->AddOutput<T>(kOutputQuotient, "Quotient");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec = args->GetInput<T>(kInputVector);
    auto scalar = args->GetInput<float>(kInputScalar);
    args->SetOutput(kOutputQuotient, *vec / *scalar);
  }
};

template <typename T>
class CrossProductNode : public BaseNode {
 public:
  enum { kInputA, kInputB };
  enum { kOutputCrossProduct };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<T>(kInputA, "A");
    node_sig->AddInput<T>(kInputB, "B");
    node_sig->AddOutput<T>(kOutputCrossProduct, "Cross Product");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec_a = args->GetInput<T>(kInputA);
    auto vec_b = args->GetInput<T>(kInputB);
    args->SetOutput(kOutputCrossProduct, T::CrossProduct(*vec_a, *vec_b));
  }
};

template <typename T>
class DotProductNode : public BaseNode {
 public:
  enum { kInputA, kInputB };
  enum { kOutputDotProduct };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<T>(kInputA, "A");
    node_sig->AddInput<T>(kInputB, "B");
    node_sig->AddOutput<float>(kOutputDotProduct, "Dot Product");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec_a = args->GetInput<T>(kInputA);
    auto vec_b = args->GetInput<T>(kInputB);
    args->SetOutput(kOutputDotProduct, vec3::DotProduct(*vec_a, *vec_b));
  }
};

template <typename T>
class LengthNode : public BaseNode {
 public:
  enum { kInputVector };
  enum { kOutputLength };

  static void OnRegister(NodeSignature* node_sig) {
    node_sig->AddInput<T>(kInputVector, "Vector");
    node_sig->AddOutput<float>(kOutputLength, "Length");
  }

  virtual void Execute(NodeArguments* args) {
    auto vec = args->GetInput<T>(kInputVector);
    args->SetOutput(kOutputLength, vec->Length());
  }
};

template <typename T>
Module* InitializeVecModuleType(ModuleRegistry* module_registry,
                                const char* name) {
  Module* module = module_registry->RegisterModule(name);
  module->RegisterNode<AddNode<T>>("add");
  module->RegisterNode<SubtractNode<T>>("subtract");
  module->RegisterNode<ScalarMultiplyNode<T>>("scalar_multiply");
  module->RegisterNode<ScalarDivideNode<T>>("scalar_divide");
  module->RegisterNode<CrossProductNode<T>>("cross_product");
  module->RegisterNode<DotProductNode<T>>("dot_product");
  module->RegisterNode<LengthNode<T>>("length");
  return module;
}

void InitializeVecModule(ModuleRegistry* module_registry) {
  // Initialize vec3 module
  TypeRegistry<vec3>::RegisterType("Vec3");
  Module* module = InitializeVecModuleType<vec3>(module_registry, "vec3");
  module->RegisterNode<Vec3Node>("vec3");
  module->RegisterNode<ElementsVec3Node>("elements");

  // Initialize vec4 module
  TypeRegistry<vec4>::RegisterType("Vec4");
  module = InitializeVecModuleType<vec3>(module_registry, "vec4");
  module->RegisterNode<Vec4Node>("vec4");
  module->RegisterNode<ElementsVec4Node>("elements");
}

}  // namespace module_library
}  // namespace breadboard
