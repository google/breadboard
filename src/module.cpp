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

#include "event/module.h"

namespace fpl {
namespace event {

NodeDef* Module::GetNodeDef(const std::string& name) {
  auto iter = node_defs_.find(name);
  if (iter == node_defs_.end()) {
    // No NodeDefs with this name, error!
    // TODO: Add logging.
    return nullptr;
  }
  return &iter->second;
}

const NodeDef* Module::GetNodeDef(const std::string& name) const {
  auto iter = node_defs_.find(name);
  if (iter == node_defs_.end()) {
    // No NodeDefs with this name, error!
    // TODO: Add logging.
    return nullptr;
  }
  return &iter->second;
}

}  // fpl
}  // event
