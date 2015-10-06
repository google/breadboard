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

#include "breadboard/event_system.h"
#include "module_library/debug.h"
#include "module_library/logic.h"
#include "module_library/math.h"
#include "module_library/string.h"

namespace fpl {
namespace module_library {

void InitializeCommonModules(breadboard::EventSystem* event_system) {
  InitializeDebugModule(event_system);
  InitializeLogicModule(event_system);
  InitializeMathModule(event_system);
  InitializeStringModule(event_system);
}

}  // namespace module_library
}  // namespace fpl
