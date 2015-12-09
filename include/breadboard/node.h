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

#ifndef BREADBOARD_NODE_H_
#define BREADBOARD_NODE_H_

#include <cassert>
#include <cstddef>
#include <vector>

#include "breadboard/type.h"

/// @file breadboard/node.h
/// @brief A Node is what graphs are built up from. Each Node defines a
/// set of connections to other Nodes between input and output edges.

namespace breadboard {

class Node;
class NodeSignature;
class BaseNode;
class OutputEdge;

/// @cond BREADBOARD_INTERNAL

/// @brief A special value representing an invalid node index.
static const unsigned int kInvalidNodeIndex = static_cast<unsigned int>(-1);

/// @brief A special value representing an invalid edge index.
static const unsigned int kInvalidEdgeIndex = static_cast<unsigned int>(-1);

/// @brief An OutputEdgeTarget represents a specific output edge on a node in a
/// graph.
///
/// Rather than point to it directly, it refers to it by index. This is
/// necessary because Nodes live in a vector, and as they are loaded they may
/// move around in memory, making it difficult to simply hold a pointer to them.
///
/// @note For internal use only.
class OutputEdgeTarget {
 public:
  OutputEdgeTarget()
      : node_index_(kInvalidNodeIndex), edge_index_(kInvalidEdgeIndex) {}

  void Initialize(unsigned int node_index, unsigned int edge_index);

  /// Given a complete list of nodes in the graph, return the node this object
  /// refers to.
  Node& GetTargetNode(std::vector<Node>* nodes) const;
  const Node& GetTargetNode(const std::vector<Node>* nodes) const;

  /// Given a complete list of nodes in the graph, return the output edge this
  /// object refers to.
  OutputEdge& GetTargetEdge(std::vector<Node>* nodes) const;
  const OutputEdge& GetTargetEdge(const std::vector<Node>* nodes) const;

  unsigned int node_index() const { return node_index_; }
  unsigned int edge_index() const { return edge_index_; }

 private:
  unsigned int node_index_;
  unsigned int edge_index_;
};

/// @brief An InputEdge represents the connection or default value of an input
/// to a Node.
///
/// An InputEdge is either connected to an output edge and points to the data in
/// that edge, or it has no connections and refers to a default value.
///
/// Because there may be multiple instances of a graph, they do not point
/// directly at the data, but rather it keeps an offset that can be used to find
/// the value in the buffer of any instance of the graph.
class InputEdge {
 public:
  InputEdge() : connected_(false), data_offset_(0) {}
  bool connected() const { return connected_; }

  ptrdiff_t data_offset() const { return data_offset_; }
  const OutputEdgeTarget& target() const { return target_; }

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

/// @brief An OutputEdge represents an edge that can pass data to other Nodes'
/// input edges.
///
/// Outputedges keep offsets to both a timestamp and to an arbitrary piece of
/// memory. Whenever the object it is pointing at is updated, it's timestamp
/// will be updated as well.
class OutputEdge {
 public:
  OutputEdge() : connected_(false), timestamp_offset_(0), data_offset_(0) {}

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

/// @brief A Node specifies the connections between nodes in a graph.
///
/// Graphs consist of any number of interconnected Nodes. Each node may have any
/// number of InputEdges and OutputEdges, and each InputEdge may connect to any
/// OutputEdge (as long as it does not form a cycle between Nodes).
class Node {
 public:
  /// @brief Construct a node from the given NodeSignature.
  ///
  /// @param[in] signature The signature for the node type this node represents.
  explicit Node(const NodeSignature* signature);

  /// @brief Return the NodeSignature for this Node.
  ///
  /// @return The NodeSignature represenging this Node.
  const NodeSignature* signature() const { return signature_; }

  /// @brief Return a pointer to the derived type holding this node's behavior.
  ///
  /// @return A pointer to the derived type holding this node's behavior.
  BaseNode* base_node() { return base_node_; }

  /// @brief Return a list of the input edges to this node.
  ///
  /// @return A list of the input edges to this node.
  std::vector<InputEdge>& input_edges() { return input_edges_; }

  /// @brief Return a list of the input edges to this node.
  ///
  /// @return A list of the input edges to this node.
  const std::vector<InputEdge>& input_edges() const { return input_edges_; }

  /// @brief Return a list of the output edges to this node.
  ///
  /// @return A list of the output edges to this node.
  std::vector<OutputEdge>& output_edges() { return output_edges_; }

  /// @brief Return a list of the input edges to this node.
  ///
  /// @return A list of the input edges to this node.
  const std::vector<OutputEdge>& output_edges() const { return output_edges_; }

  /// @brief Return a list of the NodeEventListener offsets in this node.
  ///
  /// @return A list of the NodeEventListener offsets in this node.
  std::vector<ptrdiff_t>& listener_offsets() { return listener_offsets_; }
  const std::vector<ptrdiff_t>& listener_offsets() const {
    return listener_offsets_;
  }

  /// @brief Return a list of the NodeEventListener offsets in this node.
  ///
  /// @return A list of the NodeEventListener offsets in this node.
  void set_timestamp_offset(ptrdiff_t timestamp_offset) {
    timestamp_offset_ = timestamp_offset;
  }
  ptrdiff_t timestamp_offset() const { return timestamp_offset_; }

  /// @brief Used for sorting the nodes in the graph.
  void set_inserted(bool inserted) { inserted_ = inserted; }
  /// @brief Used for sorting the nodes in the graph.
  bool inserted() { return inserted_; }
  /// @brief Used for sorting the nodes in the graph.
  void set_visited(bool visited) { visited_ = visited; }
  /// @brief Used for sorting the nodes in the graph.
  bool visited() const { return visited_; }

 private:
  const NodeSignature* signature_;
  BaseNode* base_node_;

  std::vector<InputEdge> input_edges_;
  std::vector<OutputEdge> output_edges_;
  std::vector<ptrdiff_t> listener_offsets_;

  ptrdiff_t timestamp_offset_;

  bool inserted_;
  bool visited_;
};

/// @brief Convenience function to get the type of a node's input edges.
const Type* GetInputEdgeType(const Node* node, size_t index);

/// @brief Convenience function to get the type of a node's output edges.
const Type* GetOutputEdgeType(const Node* node, size_t index);

/// @endcond

}  // namespace breadboard

#endif  // BREADBOARD_NODE_H_
