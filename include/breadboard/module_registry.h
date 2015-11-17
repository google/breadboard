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

#ifndef BREADBOARD_MODULE_REGISTRY_H_
#define BREADBOARD_MODULE_REGISTRY_H_

#include <string>
#include <unordered_map>

#include "breadboard/module.h"
#include "breadboard/version.h"

namespace breadboard {

// The event system keeps track of all registered types, modules and nodes that
// are going to be used to construct graphs.
class ModuleRegistry {
 public:
  ModuleRegistry();

  // Adds a new module with the given name to the ModuleRegistry. A module is
  // simply a collection of related nodes.
  Module* RegisterModule(const std::string& module_name);

  // Returns a pointer to a module given its name.
  const Module* GetModule(const std::string& module_name) const;

  // Returns the current Breadboard version structure.
  const BreadboardVersion* version() { return version_; }

 private:
  typedef std::unordered_map<std::string, Module> ModuleDictionary;

  // The dictionary of modules, keyed by name.
  ModuleDictionary modules_;

  // Current version of Breadboard.
  const BreadboardVersion* version_;
};

}  // namespace breadboard

#endif  // BREADBOARD_MODULE_REGISTRY_H_
