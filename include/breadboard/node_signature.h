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

#ifndef BREADBOARD_NODE_SIGNATURE_H_
#define BREADBOARD_NODE_SIGNATURE_H_

#include <functional>
#include <string>
#include <vector>

#include "breadboard/event.h"
#include "breadboard/type_registry.h"

namespace breadboard {

class BaseNode;

typedef std::function<BaseNode*()> NodeConstructor;
typedef std::function<void(BaseNode*)> NodeDestructor;

// A NodeSignature defines how many input and ouput edges a Node in a graph is
// expected to have, and what types those edges are. Additionally,
// NodeSignatures have a constructor callback which constructs an instance the
// class that implements BaseNode. The default constructor is used by default,
// but you may supply your own constructor callback if you need to do any
// special set up before creating an instance of your class.
class NodeSignature {
 public:
  NodeSignature(const std::string* module_name, const std::string& node_name,
                const NodeConstructor& constructor,
                const NodeDestructor& destructor)
      : module_name_(module_name),
        node_name_(node_name),
        constructor_(constructor),
        destructor_(destructor) {}

  const std::string* module_name() const { return module_name_; }
  const std::string& node_name() const { return node_name_; }

  // Adds intput and output edges to a node definition. The order that these
  // intputs and outputs are added matters. It determines the expected intputs
  // and outputs in the BaseNode.
  template <typename T>
  void AddInput() {
    input_types_.push_back(TypeRegistry<T>::GetType());
  }

  template <typename T>
  void AddOutput() {
    output_types_.push_back(TypeRegistry<T>::GetType());
  }

  void AddListener(const EventId event_id) {
    event_listeners_.push_back(event_id);
  }

  const std::vector<const Type*>& input_types() const { return input_types_; }
  const std::vector<const Type*>& output_types() const { return output_types_; }
  const std::vector<EventId>& event_listeners() const {
    return event_listeners_;
  }

  BaseNode* Constructor() const;
  void Destructor(BaseNode* base_node) const;

 private:
  NodeSignature();

  const std::string* module_name_;
  std::string node_name_;
  NodeConstructor constructor_;
  NodeDestructor destructor_;
  std::vector<const Type*> input_types_;
  std::vector<const Type*> output_types_;
  std::vector<EventId> event_listeners_;
};

}  // breadboard

#endif  // BREADBOARD_NODE_SIGNATURE_H_
