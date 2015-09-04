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

#ifndef FPL_EVENT_GRAPH_H_
#define FPL_EVENT_GRAPH_H_

#include <cassert>
#include <memory>
#include <vector>

#include "event/event_system.h"
#include "event/memory_buffer.h"
#include "event/node.h"

namespace fpl {
namespace event {

// A Graph is a collection of nodes that are linked together at their edges.
// Nodes in a graph may connect their input edges to other node output edges, or
// they may simply specify a default value. Setting up a graph comes in two
// distinct steps. First the nodes must be added and the connections between the
// nodes defined. Once all Nodes have been added, FinalizeNodes must be called.
// After FinalizeNodes has been called memory has been allocated in which
// default input values may be stored. It is at this point that you may call
// SetDefaultValue to set the default input values for nodes.
//
// Graphs can not execute Nodes directly. To execute behavior, a Graph object
// must be passed to a GraphState object which tracks the mutable state of the
// data in the graph. This also allows you to reuse one Graph with multiple game
// entities or actors
class Graph {
 public:
  Graph() : nodes_(), sorted_nodes_(), nodes_finalized_(false) {}
  ~Graph();

  // Add a node with the given NodeSignature to the graph. The node that was
  // just added is returned so that its edges can be set up.
  Node* AddNode(const NodeSignature* node_sig);

  // Call once after graph is set up. No more node additions or modifications
  // may be performed after this is called. Returns true if successful,
  // otherwise it logs an error and returns false.  You must finalize the nodes
  // before attempting to given nodes default values. Until the nodes are
  // finalized, the buffer that holds the default input values is not
  // initialized.
  bool FinalizeNodes();

  // Assign a default value to an unconnected input edge.  Default values may
  // only be set once the nodes are finalized using FinalizeNodes.
  template <typename T>
  void SetDefaultValue(unsigned int node_index, unsigned int edge_index,
                       const T& value) {
    assert(nodes_finalized_);
    Node& node = nodes_[node_index];
    InputEdge& input_edge = node.input_edges()[edge_index];
    T* default_object = input_buffer_.GetObject<T>(input_edge.data_offset());
    *default_object = value;
  }

  std::vector<Node>& nodes() { return nodes_; }
  const std::vector<Node>& nodes() const { return nodes_; }
  std::vector<Node*>& sorted_nodes() { return sorted_nodes_; }
  const std::vector<Node*>& sorted_nodes() const { return sorted_nodes_; }

  // Returns the memory buffer used to hold all default input buffers.
  MemoryBuffer& input_buffer() { return input_buffer_; }
  const MemoryBuffer& input_buffer() const { return input_buffer_; }

  // Returns the size that the memory buffer holding output edge data will need
  // to be.
  size_t output_buffer_size() const { return output_buffer_size_; }

  // Returns true if FinalizeNodes has been called.
  bool nodes_finalized() const { return nodes_finalized_; }

 private:
  // Utility functions used to sort the graph nodes in FinalizeNodes.
  // Returns true on success, or false if there was an error sorting the graph
  // (such as a circular).
  bool SortGraphNodes();
  bool InsertNode(Node* node);

  std::vector<Node> nodes_;
  std::vector<Node*> sorted_nodes_;
  MemoryBuffer input_buffer_;
  size_t output_buffer_size_;

  bool nodes_finalized_;
};

// A GraphState is used to execute the nodes on a graph. You can think of it as
// an instance of the graph. It contains all of the state in the output edges
// and nodes that update each time the graph executes.
class GraphState {
 public:
  GraphState() : graph_(nullptr), output_buffer_(), timestamp_(1) {}
  ~GraphState();

  // Initialize the GraphState with a graph.
  //
  // All nodes in the graph will have their Initialize function run.  The order
  // the nodes run Initialize is not specified except that nodes that have a
  // dependency on other nodes will always run after their dependencies.
  void Initialize(Graph* graph);

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
  // Return true if any of the input edges on this node point to data that has
  // been updated.
  bool IsDirty(const Node& node) const;

  Graph* graph_;
  MemoryBuffer output_buffer_;
  Timestamp timestamp_;
};

}  // event
}  // fpl

#endif  // FPL_EVENT_GRAPH_H_
