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

#ifndef BREADBOARD_TYPE_REGISTRY_H_
#define BREADBOARD_TYPE_REGISTRY_H_

#include <cassert>
#include <type_traits>

#include "breadboard/type.h"

/// @file breadboard/type_registry.h
///
/// @brief The TypeRegistry class acts as a static type id storage.

namespace breadboard {

/// @class TypeRegistry
///
/// @brief Types that are to be used as input and output edge parameters on
/// nodes must be registered with the TypeRegistry.
///
/// Typical registration is as follows:
///
/// ~~~{.cpp}
///     TypeRegistry<MyCustomType>::RegisterType("CustomType");
/// ~~~
///
/// Once a type is registered it can be used in Breadboard nodes on input and
/// output parameters. All types, even primitives like ints and floats must be
/// registered or nodes may not use them as parameters.
///
/// @note The TypeRegistry class a static consisting entirely of static fucntions
/// and is not meant to be instantiated.
template <typename EdgeType>
struct TypeRegistry {
  /// @brief Register a type with Breadboard so that it may be used as input and
  /// output edge parameters.
  ///
  /// @note By default, placement new with the default constructor is used to
  /// create an instance of the type, while a direct call to the constructor
  /// is used to destroy an instance of the type. If a custom constructor
  /// or destructor call is required, you may supply a PlacementNewFunc and
  /// OperatorDeleteFunc.
  ///
  /// @param[in] name The name of the type
  ///
  /// @param[in] placement_new_func The function to use to create a new EdgeType
  /// object
  ///
  /// @param[in] operator_delete_func The function to use to destroy a EdgeType
  /// object
  static void RegisterType(const char* name,
                           const PlacementNewFunc& placement_new_func,
                           const OperatorDeleteFunc& operator_delete_func) {
    assert(!initialized_);
    initialized_ = true;
    type_ = Type(name, sizeof(EdgeType), std::alignment_of<EdgeType>::value,
                 placement_new_func, operator_delete_func);
  }

  /// @brief Register a type with Breadboard so that it may be used as input and
  /// output edge parameters.
  ///
  /// @note By default, placement new with the default constructor is used to
  /// create an instance of the type, while a direct call to the constructor
  /// is used to destroy an instance of the type. If a custom constructor
  /// or destructor call is required, you may supply a PlacementNewFunc and
  /// OperatorDeleteFunc.
  ///
  /// @param[in] name The name of the type
  ///
  /// @param[in] placement_new_func The function to use to create a new EdgeType
  /// object
  static void RegisterType(const char* name,
                           const PlacementNewFunc& placement_new_func) {
    RegisterType(name, placement_new_func, DefaultOperatorDelete);
  }

  /// @brief Register a type with Breadboard so that it may be used as input and
  /// output edge parameters.
  ///
  /// @note By default, placement new with the default constructor is used to
  /// create an instance of the type, while a direct call to the constructor
  /// is used to destroy an instance of the type. If a custom constructor
  /// or destructor call is required, you may supply a PlacementNewFunc and
  /// OperatorDeleteFunc.
  ///
  /// @param[in] name The name of the type
  static void RegisterType(const char* name) {
    RegisterType(name, DefaultPlacementNew);
  }

  /// @brief Return the Type object that represents EdgeType.
  ///
  /// @return The Type object that represents EdgeType.
  static const Type* GetType() { return &type_; }

 private:
  static Type type_;
  static bool initialized_;

  static void DefaultPlacementNew(uint8_t* ptr) { new (ptr) EdgeType(); }

  static void DefaultOperatorDelete(uint8_t* ptr) {
    (void)ptr;
    reinterpret_cast<EdgeType*>(ptr)->~EdgeType();
  }

  TypeRegistry();
};

template <typename EdgeType>
Type TypeRegistry<EdgeType>::type_;

template <typename EdgeType>
bool TypeRegistry<EdgeType>::initialized_ = false;

/// @class TypeRegistry<void>
///
/// @brief A specialization of TypeRegistry for implementing the `void` type.
///
/// There are cases where one might want to force a node to to be marked dirty
/// without passing any data to it. Registering a void type and using that as
/// an input to a node would allow that. However, as void has no sizeof or
/// alignment and should not be allocated or deleted, it needs to be handled
/// specially.
///
/// See TypeRegistry for more details.
template <>
struct TypeRegistry<void> {
  /// Register the `void` type with the TypeRegistry so that it may be used to
  /// pass data from one node to another. No custom placement new or operator
  /// delete functions may be specified.
  static void RegisterType(const char* name) {
    assert(!initialized_);
    static const size_t kVoidSize = 0;
    static const size_t kVoidAlignment = 1;
    initialized_ = true;
    type_ = Type(name, kVoidSize, kVoidAlignment, VoidPlacementNew,
                 VoidOperatorDelete);
  }

  /// @brief Return the Type object that represents the type `void`.
  ///
  /// @return The Type object that represents the type `void`.
  static const Type* GetType() { return &type_; }

 private:
  static Type type_;
  static bool initialized_;

  // Do nothing.
  static void VoidPlacementNew(uint8_t*) {}
  static void VoidOperatorDelete(uint8_t*) {}

  TypeRegistry();
};

}  // namespace breadboard

#endif  // BREADBOARD_TYPE_REGISTRY_H_
