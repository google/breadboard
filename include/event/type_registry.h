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

#ifndef FPL_EVENT_TYPE_REGISTRY_H_
#define FPL_EVENT_TYPE_REGISTRY_H_

#include <cassert>
#include <type_traits>

#include "event/type.h"

namespace fpl {
namespace event {

// This class acts as a static type id storage, it is not meant to be
// instantiated.
template <typename T>
struct TypeRegistry {
  // Register a type with the event system so that it may be used to pass data
  // from one node to another. By default, placement new with the default
  // constructor is used. If a custom constructor call is required, you may
  // supply a PlacementNewFunc.
  static void RegisterType(const char* name,
                           const PlacementNewFunc& placement_new_func,
                           const OperatorDeleteFunc& operator_delete_func) {
    assert(!initialized_);
    initialized_ = true;
    type_ = Type(name, sizeof(T), std::alignment_of<T>::value,
                 placement_new_func, operator_delete_func);
  }

  static void RegisterType(const char* name,
                           const PlacementNewFunc& placement_new_func) {
    RegisterType(name, placement_new_func, DefaultOperatorDelete);
  }

  static void RegisterType(const char* name) {
    RegisterType(name, DefaultPlacementNew);
  }

  static const Type* GetType() { return &type_; }

 private:
  static Type type_;
  static bool initialized_;

  static void DefaultPlacementNew(uint8_t* ptr) { new (ptr) T(); }

  static void DefaultOperatorDelete(uint8_t* ptr) {
    (void)ptr;
    reinterpret_cast<T*>(ptr)->~T();
  }

  TypeRegistry();
};

template <typename T>
Type TypeRegistry<T>::type_;

template <typename T>
bool TypeRegistry<T>::initialized_ = false;

// Specialize on void. There are cases where one might want to force a node to
// to be marked dirty without passing any data to it. Registering a void type
// and using that as an input to a node would allow that. However, as void has
// no sizeof or alignment and should not be allocated or deleted, it needs to be
// handled specially.
template <>
struct TypeRegistry<void> {
  // Register a type with the event system so that it may be used to pass data
  // from one node to another. By default, placement new with the default
  // constructor is used. If a custom constructor call is required, you may
  // supply a PlacementNewFunc.
  static void RegisterType(const char* name) {
    assert(!initialized_);
    static const size_t kVoidSize = 0;
    static const size_t kVoidAlignment = 1;
    initialized_ = true;
    type_ = Type(name, kVoidSize, kVoidAlignment, VoidPlacementNew,
                 VoidOperatorDelete);
  }

  static const Type* GetType() { return &type_; }

 private:
  static Type type_;
  static bool initialized_;

  // Do nothing.
  static void VoidPlacementNew(uint8_t*) {}
  static void VoidOperatorDelete(uint8_t*) {}

  TypeRegistry();
};

}  // event
}  // fpl

#endif  // FPL_EVENT_TYPE_REGISTRY_H_
