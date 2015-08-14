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

#ifndef FPL_EVENT_EVENT_SYSTEM_H_
#define FPL_EVENT_EVENT_SYSTEM_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <type_traits>

#include "event/module.h"
#include "event/node_interface.h"
#include "event/type.h"
#include "event/type_registry.h"

namespace fpl {
namespace event {

// The event system keeps track of all registered types, modules and nodes that
// are going to be used to construct graphs.
class EventSystem {
 public:
  // Register a type with the event system so that it may be used to pass data
  // from one node to another. By default, placement new with the default
  // constructor is used. If a custom constructor call is required, you may
  // supply a PlacementNewFunc.
  template <typename T>
  void AddType(PlacementNewFunc placement_new_func,
               OperatorDeleteFunc operator_delete_func) {
    types_.push_back(std::unique_ptr<Type>(
        new Type(sizeof(T), std::alignment_of<T>::value, placement_new_func,
                 operator_delete_func)));
    TypeRegistry<T>::kType = types_.back().get();
  }

  template <typename T>
  void AddType(PlacementNewFunc placement_new_func) {
    AddType<T>(placement_new_func, DefaultOperatorDelete<T>);
  }

  template <typename T>
  void AddType() {
    AddType<T>(DefaultPlacementNew<T>);
  }

  // Adds a new module with the given name to the EventSystem. A module is
  // simply a collection of related nodes.
  Module* AddModule(const std::string& name);

  // Returns a pointer to a module given its name.
  const Module* GetModule(const std::string& name) const;

 private:
  // TODO: Consider changing over to using integer keys instead of std::strings.
  std::unordered_map<std::string, Module> modules_;
  std::vector<std::unique_ptr<Type>> types_;

  template <typename T>
  static void DefaultPlacementNew(uint8_t* ptr) {
    new (ptr) T();
  }

  template <typename T>
  static void DefaultOperatorDelete(uint8_t* ptr) {
    reinterpret_cast<T*>(ptr)->~T();
  }
};

}  // event
}  // fpl

#endif  // FPL_EVENT_EVENT_SYSTEM_H_
