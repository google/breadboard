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

#ifndef FPL_EVENT_MODULE_H_
#define FPL_EVENT_MODULE_H_

#include <string>
#include <unordered_map>

#include "event/node_def.h"
#include "event/node_interface.h"

namespace fpl {
namespace event {

// A module is a collection of related NodeDefs. For example, it may make sense
// to make a Math module for basic math operations, or an Entity module for
// entity operations.
class Module {
 public:
  // Add a new NodeDef. The template argument to this function should be a class
  // that implements NodeInterface. Each NodeInterface registered should have
  // one NodeDef associated with it. The NodeDef defines how many input and
  // output edges and the types of those edges.
  template <typename T>
  NodeDef* AddNodeDef(const std::string& name,
                      const NodeConstructor& constructor,
                      const NodeDestructor& destructor) {
    auto result = node_defs_.insert(std::pair<std::string, NodeDef>(
        name, NodeDef(constructor, destructor)));
    if (result.second == false) {
      // Already have a NodeDef with this name, error!
      // TODO: Add logging.
      return nullptr;
    }
    return &result.first->second;
  }

  template <typename T>
  NodeDef* AddNodeDef(const std::string& name,
                      const NodeConstructor& constructor) {
    return AddNodeDef<T>(name, constructor, DefaultDelete);
  }

  template <typename T>
  NodeDef* AddNodeDef(const std::string& name) {
    return AddNodeDef<T>(name, DefaultNew<T>);
  }

  NodeDef* GetNodeDef(const std::string& name);
  const NodeDef* GetNodeDef(const std::string& name) const;

 private:
  template <typename T>
  static NodeInterface* DefaultNew() {
    return new T();
  }

  static void DefaultDelete(NodeInterface* object) { delete object; }

  // TODO: Consider changing over to using integer keys instead of std::strings.
  // It's faster to look up, but looking up NodeDefs should only happen during
  // graph initialization, so it might not be worth it.
  std::unordered_map<std::string, NodeDef> node_defs_;
};

}  // event
}  // fpl

#endif  // FPL_EVENT_MODULE_H_
