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

#include "breadboard/module.h"

namespace breadboard {

NodeSignature* Module::GetNodeSignature(const std::string& name) {
  auto iter = signatures_.find(name);
  if (iter == signatures_.end()) {
    CallLogFunc("A node named \"%s\" has not been registered in module \"%s\".",
                name.c_str(), module_name_.c_str());
    return nullptr;
  }
  return &iter->second;
}

const NodeSignature* Module::GetNodeSignature(const std::string& name) const {
  auto iter = signatures_.find(name);
  if (iter == signatures_.end()) {
    CallLogFunc("A node named \"%s\" has not been registered in module \"%s\".",
                name.c_str(), module_name_.c_str());
    return nullptr;
  }
  return &iter->second;
}

}  // namespace breadboard
