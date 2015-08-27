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

#ifndef FPL_EVENT_IO_H_
#define FPL_EVENT_IO_H_

#include <vector>

#include "memory_buffer.h"
#include "node.h"

namespace fpl {
namespace event {

typedef uint32_t Timestamp;

// Inputs is a class that represent the input data passed into a node. This
// class wraps both the default values passed into a node and the values passed
// from the output of other nodes.
class Inputs {
 public:
  Inputs(const Node* node, const std::vector<Node>* nodes,
         MemoryBuffer* input_memory, MemoryBuffer* output_memory)
      : node_(node),
        nodes_(nodes),
        input_memory_(input_memory),
        output_memory_(output_memory) {}

  // Each node has number of typed inputs (specified by the NodeDef). This
  // function allows you to access the data held in any one of those inputs.
  // Typical usage would look like this:
  //
  //     Foo* foo = inputs.Get<Foo>(0);
  //     Bar* bar = inputs.Get<Bar>(1);
  //
  // Each argument is accessed by index, and the types must match the types
  // declared in the NodeDef. Care must be taken when modifying input values as
  // they will *not* be marked dirty. Since the execution order of nodes is not
  // defined, input values may change between calls to Execute on differnet
  // nodes.
  template <typename T>
  T* Get(int argument_index) const {
    // Assert that the argument index is in range.
    // TODO: Log error.
    assert(argument_index >= 0 && argument_index < node_->input_edges().size());
    const InputEdge& input_edge = node_->input_edges()[argument_index];
    if (input_edge.connected()) {
      const OutputEdgeTarget& target_edge = input_edge.target();

      // Assert that this edge is the expected type.
      // TODO: Log error.
      assert(TypeRegistry<T>::GetType() ==
             GetOutputEdgeType(&target_edge.GetTargetNode(nodes_),
                               target_edge.edge_index()));

      // Assert that this node is connected.
      // TODO: Log error.
      const OutputEdge& output_edge = target_edge.GetTargetEdge(nodes_);
      assert(output_edge.connected());

      return output_memory_->GetObject<T>(output_edge.data_offset());
    } else {
      // Assert that this edge is the right data type.
      // TODO: Log error.
      assert(TypeRegistry<T>::GetType() ==
             GetInputEdgeType(node_, argument_index));

      return input_memory_->GetObject<T>(input_edge.data_offset());
    }
  }

 private:
  const Node* node_;
  const std::vector<Node>* nodes_;
  MemoryBuffer* input_memory_;
  MemoryBuffer* output_memory_;
};

// Outputs is a class that represent the outputs data passed to other nodes.
// This class wraps mutable buffer that holds values that other nodes connect
// to.
class Outputs {
 public:
  Outputs(const Node* node, MemoryBuffer* memory, Timestamp timestamp)
      : node_(node), timestamp_(timestamp), memory_(memory) {}

  // Each node has number of typed outputs (specified by the NodeDef). This
  // function allows you to pass data through an OutputEdge so that it is
  // accessible to any InputNode that is connected to it.
  // Typical usage would look like this:
  //
  //     Foo foo = ...
  //     Bar bar = ...
  //     outputs.Set<Foo>(0, foo);
  //     outputs.Set<Bar>(1, bar);
  //
  // Each ouput is accessed by index, and the types must match the types
  // declared in the NodeDef. When you set a value, it is given a timestamp
  // which marks that node as dirty, meaning that any node that relies on it
  // will re-run its Execute function. If an edge is not connected to any
  // inputs, the value is discarded and this function call does nothing.
  template <typename T>
  void Set(int argument_index, const T& value) {
    // Assert that the argument index is in range.
    // TODO: Log error.
    assert(argument_index >= 0 &&
           argument_index < node_->output_edges().size());

    // Assert that this edge is the right data type.
    // TODO: Log error.
    assert(TypeRegistry<T>::GetType() ==
           GetOutputEdgeType(node_, argument_index));

    const OutputEdge& output_edge = node_->output_edges()[argument_index];
    if (!output_edge.connected()) {
      // Nothing is consuming this output, so no need to store it.
      return;
    }

    // Mark that this value has changed.
    Timestamp* timestamp =
        memory_->GetObject<Timestamp>(output_edge.timestamp_offset());
    *timestamp = timestamp_;

    T* data = memory_->GetObject<T>(output_edge.data_offset());
    *data = value;
  }

 private:
  const Node* node_;
  const Timestamp timestamp_;
  MemoryBuffer* memory_;
};

}  // event
}  // fpl

#endif  // FPL_EVENT_IO_H_
