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

#ifndef BREADBOARD_GRAPH_STATE_H_
#define BREADBOARD_GRAPH_STATE_H_

#include <cassert>
#include <memory>
#include <vector>

#include "breadboard/graph.h"
#include "breadboard/memory_buffer.h"
#include "breadboard/node.h"

/// @file breadboard/graph_state.h
///
/// @brief A GraphState represents an instance of a Graph, and can be connected
///        to game objects to script and execute behaviors.

namespace breadboard {

/// @class GraphState
///
/// @brief A GraphState represents an instance of a Graph, and can be connected
///        to game objects to script and execute behaviors.
///
/// Once a GraphState has been initialized with a Graph, it can be used to
/// execute various scripted behaviors.
class GraphState {
 public:
  /// @brief The default constructor for an empty GraphState object.
  GraphState() : graph_(nullptr), output_buffer_(), timestamp_(0) {}

  /// @brief Destructor for a BaseNode.
  ~GraphState();

  /// @brief Initialize the GraphState with a graph.
  ///
  /// All nodes in the graph will have their Initialize function run.  The order
  /// the nodes run Initialize is not specified except that nodes that have a
  /// dependency on other nodes will always run after their dependencies.
  ///
  /// @param[in] graph The Graph that defines this GraphState's nodes and edges.
  void Initialize(Graph* graph);

  /// @brief Check if this GraphState has been initialized.
  ///
  /// @return Whether or not the Graph has been initialized.
  bool IsInitialized() const { return graph_ != nullptr; }

  /// @cond BREADBOARD_INTERNAL

  /// @brief Execute all Nodes that are considered 'dirty'.
  ///
  /// A dirty node is any node that has been updated since the last time Execute
  /// was called, or one that has an input edge pointing at data that has
  /// changed. Nodes that do not meet either of these requirements do not have
  /// their Execute function called.
  ///
  /// The order the nodes run Execute is not specified except that nodes that
  /// have a dependency on other nodes will always run after their dependencies.
  ///
  /// @note This is for internal use only.
  void Execute();

  /// @brief Return the current timestamp.
  ///
  /// @note This is for internal use only.
  ///
  /// @return the current timestamp.
  Timestamp timestamp() const { return timestamp_; }

  /// @brief Returns the internal memory buffer for output edge objects.
  ///
  /// @note This is for internal use only.
  ///
  /// @return the internal memory buffer for output edge objects.
  MemoryBuffer* output_buffer() { return &output_buffer_; }

  /// @brief Returns the internal memory buffer for output edge objects.
  ///
  /// @note This is for internal use only.
  ///
  /// @return the internal memory buffer for output edge objects.
  const MemoryBuffer* output_buffer() const { return &output_buffer_; }

  /// @endcond

 private:
  // Disallow copying.
  GraphState(GraphState&);
  GraphState& operator=(GraphState&);
  GraphState(GraphState&&);
  GraphState& operator=(GraphState&&);

  // Return true if any of the input edges on this node point to data that has
  // been updated.
  bool IsDirty(const Node& node) const;

  Graph* graph_;
  MemoryBuffer output_buffer_;
  Timestamp timestamp_;
};

}  // namespace breadboard

#endif  // BREADBOARD_GRAPH_STATE_H_
