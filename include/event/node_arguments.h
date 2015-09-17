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

#ifndef FPL_EVENT_NODE_ARGUMENTS_H_
#define FPL_EVENT_NODE_ARGUMENTS_H_

#include <vector>

#include "event/log.h"
#include "event/memory_buffer.h"
#include "event/node.h"
#include "event/type_registry.h"

namespace fpl {
namespace event {

typedef uint32_t Timestamp;

// NodeArguments is a class that represent the input and output edges connected
// to a node. Functionality is provided to get inputs and set outputs.
// Additionally, this contains the list of listeners on a given node so that you
// can connect a listener to a broadcaster.
class NodeArguments {
 public:
  NodeArguments(const Node* node, const std::vector<Node>* nodes,
                MemoryBuffer* input_memory, MemoryBuffer* output_memory,
                Timestamp timestamp)
      : node_(node),
        nodes_(nodes),
        input_memory_(input_memory),
        output_memory_(output_memory),
        timestamp_(timestamp) {}

  // Each node has number of typed inputs (specified by the NodeDef). This
  // function allows you to access the data held in any one of those inputs.
  // Typical usage would look like this:
  //
  //     Foo* foo = args->GetInput<Foo>(0);
  //     Bar* bar = args->GetInput<Bar>(1);
  //
  // Each argument is accessed by index, and the types must match the types
  // declared in the NodeDef. Care must be taken when modifying input values as
  // they will *not* be marked dirty. Since the execution order of nodes is not
  // defined, input values may change between calls to Execute on differnet
  // nodes.
  template <typename T>
  T* GetInput(size_t argument_index) const {
    VerifyInputPreconditions(argument_index, TypeRegistry<T>::GetType());

    const InputEdge& input_edge = node_->input_edges()[argument_index];
    if (input_edge.connected()) {
      const OutputEdgeTarget& target_edge = input_edge.target();
      const OutputEdge& output_edge = target_edge.GetTargetEdge(nodes_);
      return output_memory_->GetObject<T>(output_edge.data_offset());
    } else {
      return input_memory_->GetObject<T>(input_edge.data_offset());
    }
  }

  // Each node has number of typed outputs (specified by the NodeDef). This
  // function allows you to pass data through an OutputEdge so that it is
  // accessible to any InputNode that is connected to it.
  // Typical usage would look like this:
  //
  //     Foo foo = ...
  //     Bar bar = ...
  //     outputs.SetOutput(0, foo);
  //     outputs.SetOutput(1, bar);
  //
  // Each ouput is accessed by index, and the types must match the types
  // declared in the NodeDef. When you set a value, it is given a timestamp
  // which marks that node as dirty, meaning that any node that relies on it
  // will re-run its Execute function. If an edge is not connected to any
  // inputs, the value is discarded and this function call does nothing.
  template <typename T>
  void SetOutput(size_t argument_index, const T& value) {
    VerifyOutputPreconditions(argument_index, TypeRegistry<T>::GetType());

    const OutputEdge& output_edge = node_->output_edges()[argument_index];
    if (!output_edge.connected()) {
      // Nothing is consuming this output, so no need to store it.
      return;
    }

    // Mark that this value has changed.
    Timestamp* timestamp =
        output_memory_->GetObject<Timestamp>(output_edge.timestamp_offset());
    *timestamp = timestamp_;

    T* data = output_memory_->GetObject<T>(output_edge.data_offset());
    *data = value;
  }

  // This is a special overload of SetOutput for void output types, which are
  // typically used just to send a signal to another node but which carry no
  // data. With this version of the function you do not need to supply any
  // value. Typical usage would look like this:
  //
  //     outputs.SetOutput(0);
  //     outputs.SetOutput(1);
  void SetOutput(size_t argument_index) {
    VerifyOutputPreconditions(argument_index, TypeRegistry<void>::GetType());

    const OutputEdge& output_edge = node_->output_edges()[argument_index];
    if (!output_edge.connected()) {
      // Nothing is consuming this output, so no need to store it.
      return;
    }

    // Mark that this value has changed.
    Timestamp* timestamp =
        output_memory_->GetObject<Timestamp>(output_edge.timestamp_offset());
    *timestamp = timestamp_;
  }

  void BindBroadcaster(size_t listener_index,
                       NodeEventBroadcaster* broadcaster) {
    NodeEventListener* listener = output_memory_->GetObject<NodeEventListener>(
        node_->listener_offsets()[listener_index]);
    broadcaster->RegisterListener(listener);
  }

 private:
  // Check to make sure the argument index is in range and the type being
  // retrieved is the type expected.
  void VerifyInputPreconditions(size_t argument_index,
                                const Type* requested_type) const {
    if (argument_index >= node_->input_edges().size()) {
      CallLogFunc(
          "Attempting to get argument %i when node only has %i input edges.",
          argument_index, static_cast<int>(node_->input_edges().size()));
      assert(0);
    }
    const Type* expected_type = GetInputEdgeType(node_, argument_index);
    if (requested_type != expected_type) {
      CallLogFunc(
          "Attempting to get input argument %i as type \"%s\" when it expects "
          "type \"%s\".",
          argument_index, requested_type->name, expected_type->name);
      assert(0);
    }
  }

  // Check to make sure the argument index is in range and the type being set is
  // the type expected.
  void VerifyOutputPreconditions(size_t argument_index,
                                 const Type* requested_type) const {
    if (argument_index >= node_->output_edges().size()) {
      CallLogFunc(
          "Attempting to get argument %i when node only has %i output edges.",
          argument_index, static_cast<int>(node_->input_edges().size()));
      assert(0);
    }
    const Type* expected_type = GetOutputEdgeType(node_, argument_index);
    if (requested_type != expected_type) {
      CallLogFunc(
          "Attempting to set output argument %i as type \"%s\" when it expects "
          "type \"%s\".",
          argument_index, requested_type->name, expected_type->name);
      assert(0);
    }
  }

 private:
  const Node* node_;
  const std::vector<Node>* nodes_;
  MemoryBuffer* input_memory_;
  MemoryBuffer* output_memory_;
  Timestamp timestamp_;
};

}  // event
}  // fpl

#endif  // FPL_EVENT_NODE_ARGUMENTS_H_
