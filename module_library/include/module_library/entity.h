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

#ifndef FPL_BREADBOARD_MODULE_LIBRARY_ENTITY_H_
#define FPL_BREADBOARD_MODULE_LIBRARY_ENTITY_H_

#include "breadboard/module_registry.h"
#include "corgi_component_library/graph.h"
#include "corgi_component_library/meta.h"
#include "corgi/entity_manager.h"

namespace breadboard {
namespace module_library {

void SetGraphEntity(::corgi::EntityRef entity);

void InitializeEntityModule(
    breadboard::ModuleRegistry* module_registry,
    ::corgi::EntityManager* entity_manager,
    ::corgi::component_library::MetaComponent* meta_component,
    ::corgi::component_library::GraphComponent* graph_component);

}  // namespace module_library
}  // namespace breadboard

#endif  // FPL_BREADBOARD_MODULE_LIBRARY_ENTITY_H_
