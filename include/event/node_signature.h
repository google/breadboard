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

#ifndef FPL_EVENT_NODE_SIGNATURE_H_
#define FPL_EVENT_NODE_SIGNATURE_H_

#include <functional>
#include <vector>

#include "event/type_registry.h"

namespace fpl {
namespace event {

class NodeInterface;

typedef std::function<NodeInterface*()> NodeConstructor;
typedef std::function<void(NodeInterface*)> NodeDestructor;

// A NodeSignature defines how many input and ouput edges a Node in a graph is
// expected to have, and what types those edges are. Additionally,
// NodeSignatures have a constructor callback which constructs an instance the
// class that implements NodeInterface. The default constructor is used by
// default, but you may supply your own constructor callback if you need to do
// any special set up before creating an instance of your class.
class NodeSignature {
 public:
  NodeSignature(const NodeConstructor& constructor,
                const NodeDestructor& destructor)
      : constructor_(constructor), destructor_(destructor) {}

  // Adds intput and output edges to a node definition. The order that these
  // intputs and outputs are added matters. It determines the expected intputs
  // and outputs in the NodeInterface.
  template <typename T>
  void AddInput() {
    input_types_.push_back(TypeRegistry<T>::GetType());
  }

  template <typename T>
  void AddOutput() {
    output_types_.push_back(TypeRegistry<T>::GetType());
  }

  const std::vector<const Type*>& input_types() const { return input_types_; }
  const std::vector<const Type*>& output_types() const { return output_types_; }

  NodeInterface* Constructor() const;
  void Destructor(NodeInterface* node_interface) const;

 private:
  NodeConstructor constructor_;
  NodeDestructor destructor_;
  std::vector<const Type*> input_types_;
  std::vector<const Type*> output_types_;
};

}  // event
}  // fpl

#endif  // FPL_EVENT_NODE_SIGNATURE_H_
