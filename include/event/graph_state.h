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

#ifndef FPL_EVENT_GRAPH_STATE_H_
#define FPL_EVENT_GRAPH_STATE_H_

#include <cassert>
#include <memory>
#include <vector>

#include "event/graph.h"
#include "event/memory_buffer.h"
#include "event/node.h"

namespace fpl {
namespace event {

// A GraphState is used to execute the nodes on a graph. You can think of it as
// an instance of the graph. It contains all of the state in the output edges
// and nodes that update each time the graph executes.
class GraphState {
 public:
  GraphState()
      : graph_(nullptr), output_buffer_(), timestamp_(kInitialTimestamp) {}
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
  // Nodes are considered dirty when the node's timestamp is equal to the the
  // graph's timestamp. We don't want nodes considered dirty the first time they
  // execute as that would trigger every single node behavior at once.  Instead,
  // we start the graph with a timestamp of 1. Any initial set up that should
  // happen should be taken care of in the Initalize function.
  static const Timestamp kInitialTimestamp = 1;

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

}  // event
}  // fpl

#endif  // FPL_EVENT_GRAPH_STATE_H_
