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

/// @file breadboard/module_registry.h
/// @brief The ModuleRegistry is a collection of Modules.

namespace breadboard {

/// @class ModuleRegistry
///
/// @brief The ModuleRegistry is a collection of Modules.
///
/// This acts as a central repository for all modules used in your project.
class ModuleRegistry {
 public:
  /// Construct a ModuleRegistry.
  ModuleRegistry();

  /// @brief Adds a new module with the given name to the ModuleRegistry.
  ///
  /// @param[in] module_name The name of the new Module.
  ///
  /// @return The new Module. Returns null if the module could not be created.
  Module* RegisterModule(const std::string& module_name);

  /// @brief Returns a pointer to a module given its name.
  ///
  /// @param[in] module_name The name of the Module.
  ///
  /// @return a pointer to a module given its name.
  const Module* GetModule(const std::string& module_name) const;

  /// @brief Returns the current Breadboard version structure.
  ///
  /// @return The current Breadboard version structure.
  const BreadboardVersion* version() const { return version_; }

 private:
  typedef std::unordered_map<std::string, Module> ModuleDictionary;

  // The dictionary of modules, keyed by name.
  ModuleDictionary modules_;

  // Current version of Breadboard.
  const BreadboardVersion* version_;
};

}  // namespace breadboard

#endif  // BREADBOARD_MODULE_REGISTRY_H_
