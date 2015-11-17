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

namespace breadboard {

// A GraphState is used to execute the nodes on a graph. You can think of it as
// an instance of the graph. It contains all of the state in the output edges
// and nodes that update each time the graph executes.
class GraphState {
 public:
  GraphState() : graph_(nullptr), output_buffer_(), timestamp_(0) {}
  ~GraphState();

  // Initialize the GraphState with a graph.
  //
  // All nodes in the graph will have their Initialize function run.  The order
  // the nodes run Initialize is not specified except that nodes that have a
  // dependency on other nodes will always run after their dependencies.
  void Initialize(Graph* graph);

  // Check if this GraphState has been initialized.
  bool IsInitialized() const { return graph_ != nullptr; }

  // Execute all Nodes that are considered 'dirty'. A dirty node is any node
  // that has been updated since the last time Execute was called, or one that
  // has an input edge pointing at data that has changed. Nodes that do not meet
  // either of these requirements do not have their Execute function called.
  //
  // The order the nodes run Execute is not specified except that nodes that
  // have a dependency on other nodes will always run after their dependencies.
  void Execute();

  // Return the current timestamp.
  Timestamp timestamp() const { return timestamp_; }

  // Get the internal memory buffer for output edge objects.
  MemoryBuffer* output_buffer() { return &output_buffer_; }
  const MemoryBuffer* output_buffer() const { return &output_buffer_; }

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
