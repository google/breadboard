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

#include "event/event_system.h"
#include "event/log.h"

namespace fpl {
namespace event {

Module* EventSystem::AddModule(const std::string& name) {
  auto iter = modules_.find(name);
  if (iter != modules_.end()) {
    CallLogFunc("A module named \"%s\" has already been registered.",
                name.c_str());
    return nullptr;
  }
  return &modules_[name];
}

const Module* EventSystem::GetModule(const std::string& name) const {
  auto iter = modules_.find(name);
  if (iter == modules_.end()) {
    CallLogFunc("No module named \"%s\" has been registered.", name.c_str());
    return nullptr;
  }
  return &iter->second;
}

}  // fpl
}  // event
