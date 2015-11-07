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

#ifndef BREADBOARD_NODE_ARGUMENTS_H_
#define BREADBOARD_NODE_ARGUMENTS_H_

#include <vector>

#include "breadboard/event.h"
#include "breadboard/log.h"
#include "breadboard/memory_buffer.h"
#include "breadboard/node.h"
#include "breadboard/type_registry.h"

/// @file breadboard/node_arguments.h
///
/// @brief NodeArguments is a class representing the state of all inputs and
/// outpus to a given node.

namespace breadboard {

/// @class NodeArguments
///
/// @brief NodeArguments is a class that represents the state of all input and
/// output edges connected to a node.
///
/// Functionality is provided to get inputs and set outputs. Additionally, this
/// contains the list of listeners on a given node so that you can connect a
/// listener to a broadcaster.
class NodeArguments {
 public:
  /// @cond BREADBOARD_INTERNAL
  /// @brief Construct a NodeArguments object for the given node.
  ///
  /// @note For internal use only.
  ///
  /// @param[in] node The node these arguments refer to.
  ///
  /// @param[in] nodes The list of nodes on the graph.
  ///
  /// @param[in] input_memory The MemoryBuffer for the input edges.
  ///
  /// @param[in] output_memory The MemoryBuffer for the output edges.
  ///
  /// @param[in] timestamp The current timestamp.
  NodeArguments(const Node* node, const std::vector<Node>* nodes,
                MemoryBuffer* input_memory, MemoryBuffer* output_memory,
                Timestamp timestamp)
      : node_(node),
        nodes_(nodes),
        input_memory_(input_memory),
        output_memory_(output_memory),
        timestamp_(timestamp) {}
  /// @endcond BREADBOARD_INTERNAL

  /// @brief Returns the value of this input edge.
  ///
  /// Each node has number of typed inputs (specified by the
  /// NodeSignature). This function allows you to access the data held in any
  /// one of those inputs.  Typical usage would look like this:
  ///
  /// ~~~{.cpp}
  ///     Foo* foo = args->GetInput<Foo>(0);
  ///     Bar* bar = args->GetInput<Bar>(1);
  /// ~~~
  ///
  /// Each argument is accessed by index, and the types must match the types
  /// declared in the NodeSignature. Care must be taken when modifying input
  /// values as they will *not* be marked dirty. Since the execution order of
  /// nodes is not defined, input values may change between calls to Execute on
  /// different nodes.
  ///
  /// @param argument_index The index of the input edge. Node that the template
  /// argument of this index must match the one specified in the NodeSignature.
  ///
  /// @return A pointer to the value of the given input edge.
  template <typename EdgeType>
  EdgeType* GetInput(size_t argument_index) const {
    VerifyInputPreconditions(argument_index, TypeRegistry<EdgeType>::GetType());

    const InputEdge& input_edge = node_->input_edges()[argument_index];
    if (input_edge.connected()) {
      const OutputEdgeTarget& target_edge = input_edge.target();
      const OutputEdge& output_edge = target_edge.GetTargetEdge(nodes_);
      return output_memory_->GetObject<EdgeType>(output_edge.data_offset());
    } else {
      return input_memory_->GetObject<EdgeType>(input_edge.data_offset());
    }
  }

  /// @brief Returns true if the given input argument index has been modified
  /// since the last execution of this graph.
  ///
  /// @return True if the given input argument index has been modified since the
  /// last execution of this graph.
  bool IsInputDirty(size_t argument_index) const;

  /// @brief Returns true if the given listener index has been signaled by a
  /// broadcaster since the last execution of this graph.
  ///
  /// @return True if the given listener index has been signaled by a
  /// broadcaster since the last execution of this graph.
  bool IsListenerDirty(size_t listener_index) const;

  /// @brief Sets the value of the specified output edge.
  ///
  /// Each node has number of typed outputs (specified by the NodeSignature).
  /// This function allows you to pass data through an output edge so that it is
  /// accessible to any input edge that is connected to it.
  ///
  /// Typical usage would look like this:
  ///
  /// ~~~{.cpp}
  ///     Foo foo = ...
  ///     Bar bar = ...
  ///     outputs.SetOutput(0, foo);
  ///     outputs.SetOutput(1, bar);
  /// ~~~
  ///
  /// Each output is accessed by index, and the types must match the types
  /// declared in the NodeSignature. When you set a value that edge is marked
  /// dirty, meaning that any node that relies on it will re-run its Execute
  /// function. If an edge is not connected to any inputs, the value is
  /// discarded and this function call does nothing.
  ///
  /// @param argument_index The index of the input edge. Node that the template
  /// argument of this index must match the one specified in the NodeSignature.
  ///
  /// @param value The value to set this output edge to.
  template <typename EdgeType>
  void SetOutput(size_t argument_index, const EdgeType& value) {
    VerifyOutputPreconditions(argument_index,
                              TypeRegistry<EdgeType>::GetType());

    const OutputEdge& output_edge = node_->output_edges()[argument_index];
    if (!output_edge.connected()) {
      // Nothing is consuming this output, so no need to store it.
      return;
    }

    // Mark that this value has changed.
    Timestamp* timestamp =
        output_memory_->GetObject<Timestamp>(output_edge.timestamp_offset());
    *timestamp = timestamp_;

    EdgeType* data =
        output_memory_->GetObject<EdgeType>(output_edge.data_offset());
    *data = value;
  }

  /// @brief Marks an output edge as dirty without updating its value.
  ///
  /// This is a special overload of SetOutput for void output types, which are
  /// typically used just to send a signal to another node but which carry no
  /// data. With this version of the function you do not need to supply any
  /// value. Typical usage would look like this:
  ///
  /// ~~~{.cpp}
  ///     outputs.SetOutput(0);
  ///     outputs.SetOutput(1);
  /// ~~~
  ///
  /// @param argument_index The index of the input edge. Node that the template
  /// argument of this index must be `void`.
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

  /// @brief Binds the given broadcasater to the listener at the given index.
  ///
  /// An NodeEventListener cannot respond to events until it has been bound to a
  /// NodeEventBroadcaster. Use this function to bind a broadcaster to a
  /// listener.
  ///
  /// @param listener_index The index of the listener.
  ///
  /// @param broadcaster The broadcaster to bind the given listener to.
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
                                const Type* requested_type) const;

  // Check to make sure the argument index is in range and the type being set is
  // the type expected.
  void VerifyOutputPreconditions(size_t argument_index,
                                 const Type* requested_type) const;

  void VerifyListenerPreconditions(size_t listener_index) const;

  const Node* node_;
  const std::vector<Node>* nodes_;
  MemoryBuffer* input_memory_;
  MemoryBuffer* output_memory_;
  Timestamp timestamp_;
};

}  // namespace breadboard

#endif  // BREADBOARD_NODE_ARGUMENTS_H_
