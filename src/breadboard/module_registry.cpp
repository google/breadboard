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

#include "breadboard/module_registry.h"
#include "breadboard/log.h"
#include "breadboard/version.h"

namespace breadboard {

ModuleRegistry::ModuleRegistry() : version_(&Version()) {}

Module* ModuleRegistry::RegisterModule(const std::string& module_name) {
  auto result =
      modules_.insert(std::make_pair(module_name, Module(module_name)));
  ModuleDictionary::iterator iter = result.first;

  bool success = result.second;
  if (!success) {
    CallLogFunc("A module named \"%s\" has already been registered.",
                module_name.c_str());
    return nullptr;
  }

  Module* module = &iter->second;
  return module;
}

const Module* ModuleRegistry::GetModule(const std::string& module_name) const {
  auto iter = modules_.find(module_name);
  if (iter == modules_.end()) {
    CallLogFunc("No module named \"%s\" has been registered.",
                module_name.c_str());
    return nullptr;
  }
  return &iter->second;
}

}  // namespace breadboard
