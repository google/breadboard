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

#ifndef FPL_EVENT_NODE_H_
#define FPL_EVENT_NODE_H_

#include <cassert>
#include <cstddef>
#include <vector>

#include "event/type.h"

namespace fpl {
namespace event {

class Node;
class NodeSignature;
class NodeInterface;
class OutputEdge;

// An OutputEdgeTarget represents a specific output edge on a node in a graph.
// Rather than point to it directly, it refers to it by index. This is necessary
// because Nodes live in a vector, and as they are loaded they may move around
// in memory, making it difficult to simply hold a pointer to them.
class OutputEdgeTarget {
 public:
  void Initialize(unsigned int node_index, unsigned int edge_index);

  // Given a complete list of nodes in the graph, return the node this object
  // refers to.
  Node& GetTargetNode(std::vector<Node>* nodes) const;
  const Node& GetTargetNode(const std::vector<Node>* nodes) const;

  // Given a complete list of nodes in the graph, return the output edge this
  // object refers to.
  OutputEdge& GetTargetEdge(std::vector<Node>* nodes) const;
  const OutputEdge& GetTargetEdge(const std::vector<Node>* nodes) const;

  unsigned int node_index() const { return node_index_; }
  unsigned int edge_index() const { return edge_index_; }

 private:
  unsigned int node_index_;
  unsigned int edge_index_;
};

// An InputEdge represents the connection or default value of an input to a
// Node. An InputEdge is either connected to an output edge and points to the
// data in that edge, or it has no connections and refers to a default value.
//
// Because there may be multiple instances of a graph, they do not point
// directly at the data, but rather it keeps an offset that can be used to find
// the value in the buffer of any instance of the graph.
class InputEdge {
 public:
  bool connected() const { return connected_; }

  ptrdiff_t data_offset() const { return data_offset_; };
  const OutputEdgeTarget& target() const { return target_; };

  void SetTarget(unsigned int node_index, unsigned int output_index) {
    connected_ = true;
    target_.Initialize(node_index, output_index);
  }

  void SetDataOffset(ptrdiff_t data_offset) {
    assert(!connected_);
    data_offset_ = data_offset;
  }

 private:
  bool connected_;

  // If connected to an input node, data needs to be allocated for it:
  ptrdiff_t data_offset_;

  // If connected to an output node, just point at the output.
  OutputEdgeTarget target_;
};

// An OutputEdge represents an edge that can pass data to other Nodes' input
// edges. Outputedges keep offsets to both a timestamp and to an arbitrary piece
// of memory. Whenever the object it is pointing at is updated, it's timestamp
// will be updated as well.
class OutputEdge {
 public:
  OutputEdge() : connected_(false), timestamp_offset_(), data_offset_() {}

  bool connected() const { return connected_; }
  void set_connected(bool connected) { connected_ = connected; }

  void set_timestamp_offset(ptrdiff_t timestamp_offset) {
    timestamp_offset_ = timestamp_offset;
  }
  ptrdiff_t timestamp_offset() const { return timestamp_offset_; }

  void set_data_offset(ptrdiff_t data_offset) { data_offset_ = data_offset; }
  ptrdiff_t data_offset() const { return data_offset_; }

 private:
  bool connected_;

  ptrdiff_t timestamp_offset_;
  ptrdiff_t data_offset_;
};

// A Node specifies the connections between nodes in a graph. Graphs consist of
// any number of interconnected Nodes. Each node may have any number of
// InputEdges and OutputEdges, and each InputEdge may connect to any OutputEdge
// (as long as it does not form a cycle between Nodes).
class Node {
 public:
  Node(const NodeSignature* node_sig);

  const NodeSignature* node_sig() const { return node_sig_; }
  NodeInterface* node_interface() { return node_interface_; }

  std::vector<InputEdge>& input_edges() { return input_edges_; }
  const std::vector<InputEdge>& input_edges() const { return input_edges_; }

  std::vector<OutputEdge>& output_edges() { return output_edges_; }
  const std::vector<OutputEdge>& output_edges() const { return output_edges_; }

  void set_timestamp_offset(ptrdiff_t timestamp_offset) {
    timestamp_offset_ = timestamp_offset;
  }
  ptrdiff_t timestamp_offset() const { return timestamp_offset_; }

  // Used for sorting the nodes in the graph.
  void set_inserted(bool inserted) { inserted_ = inserted; }
  bool inserted() { return inserted_; }
  void set_visited(bool visited) { visited_ = visited; }
  bool visited() const { return visited_; }

 private:
  const NodeSignature* node_sig_;
  NodeInterface* node_interface_;

  std::vector<InputEdge> input_edges_;
  std::vector<OutputEdge> output_edges_;

  ptrdiff_t timestamp_offset_;

  bool inserted_;
  bool visited_;
};

// Convenience function to get the type of a node's input edges.
const Type* GetInputEdgeType(const Node* node, int index);

// Convenience function to get the type of a node's output edges.
const Type* GetOutputEdgeType(const Node* node, int index);

}  // event
}  // fpl

#endif  // FPL_EVENT_NODE_H_
