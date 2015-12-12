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

#ifndef BREADBOARD_GRAPH_H_
#define BREADBOARD_GRAPH_H_

#include <cassert>
#include <memory>
#include <vector>

#include "breadboard/log.h"
#include "breadboard/memory_buffer.h"
#include "breadboard/node.h"
#include "breadboard/node_signature.h"

/// @file breadboard/graph.h
///
/// @brief A Graph is a collection of nodes that are linked together at their
///        edges.

namespace breadboard {

/// @class Graph
///
/// @brief A Graph represents the relationship between a variety of nodes. It
///        contains a collection of nodes that are linked together by their
///        edges.
///
/// Nodes in a graph may connect their input edges to the output edges of other
/// nodes or they may simply specify a default value.
///
/// Graphs cannot execute nodes directly. To have nodes executed, a Graph object
/// must be passed to a GraphState object which tracks the mutable state of the
/// data in the graph. This also allows you to reuse one Graph with multiple
/// game entities or actors. Once a GraphState has been initialized, events
/// broadcast to the nodes in the graph will trigger the nodes in the graph.
class Graph {
 public:
  /// @brief Construct a graph.
  ///
  /// @param[in] graph_name An arbitrary string used to identify this graph.
  ///            Typically this is just the filename of the file that loaded
  ///            this Graph, but this is not required. This string shows up in
  ///            the error logs if there are errors with the graph.
  explicit Graph(const std::string& graph_name)
      : graph_name_(graph_name),
        nodes_(),
        sorted_nodes_(),
        nodes_finalized_(false) {}

  /// @brief Destructor for a BaseNode.
  ~Graph();

  /// @brief Construct a graph.
  ///
  /// @return Returns the name of this Graph.
  const std::string& graph_name() const { return graph_name_; }

  /// @brief Add a new node with the given signature to the Graph.
  ///
  /// @param[in] signature The signature of the new node to add to the graph.
  ///
  /// @return Returns the node that was just added so that its input edges can
  ///         be set up.
  Node* AddNode(const NodeSignature* signature);

  /// @brief FinalizeNodes should be called once, after all nodes have been
  ///        added to the Graph, and before setting default input edge values.
  ///
  /// Setting up a graph comes in two distinct steps. First the nodes must be
  /// added and the connections between the nodes defined. Once all Nodes have
  /// been added, FinalizeNodes must be called. After FinalizeNodes has been
  /// called memory has been allocated in which default input values may be
  /// stored. It is at this point that you may call SetDefaultValue to set the
  /// default input values for nodes.
  ///
  /// Call once after graph is set up. No more node additions or modifications
  /// may be performed after this is called.
  ///
  /// @return Returns true if successful. Otherwise an error is logged and false
  //          is returned.
  bool FinalizeNodes();

  /// @brief Returns true if FinalizeNodes has been called.
  ///
  /// @return Returns true if FinalizeNodes has been called.
  bool nodes_finalized() const { return nodes_finalized_; }

  /// @brief Set the default input edge argument for a node.
  ///
  /// Default values may only be set once the nodes are finalized using
  /// FinalizeNodes.
  ///
  /// @param[in] node_index The index of the node.
  /// @param[in] edge_index The index of the edge on the node.
  /// @param[in] value The value to set the given input edge to. The templated
  ///            type of this value must match the expected type, defined in the
  ///            NodeSignature.
  template <typename EdgeType>
  void SetDefaultValue(unsigned int node_index, unsigned int edge_index,
                       const EdgeType& value) {
    assert(nodes_finalized_);
    if (node_index >= nodes_.size()) {
      CallLogFunc(
          "%s: Attempting to assign a default value on node %d when graph only "
          "has %d nodes.",
          graph_name_.c_str(), node_index, static_cast<int>(nodes_.size()));
      return;
    }
    Node& node = nodes_[node_index];
    const NodeSignature* signature = node.signature();
    if (edge_index >= node.input_edges().size()) {
      CallLogFunc(
          "%s: Attempting to assign a default value to node %i (%s:%s), edge "
          "%d when node only has %d input edges.",
          graph_name_.c_str(), node_index, signature->module_name()->c_str(),
          signature->node_name().c_str(), edge_index,
          static_cast<int>(node.input_edges().size()));
      return;
    }
    const Type* type = TypeRegistry<EdgeType>::GetType();
    const Type* expected_type = signature->input_parameters()[edge_index].type;
    if (type != expected_type) {
      CallLogFunc(
          "%s: Attempting to assign a default value of the incorrect type to "
          "node %d (%s:%s), edge %d. Edge got type \"%s\" when it expects type "
          "\"%s\".",
          graph_name_.c_str(), node_index, signature->module_name()->c_str(),
          signature->node_name().c_str(), edge_index, type->name,
          expected_type->name);
      return;
    }
    InputEdge& input_edge = node.input_edges()[edge_index];
    EdgeType* default_object =
        input_buffer_.GetObject<EdgeType>(input_edge.data_offset());
    *default_object = value;
  }

  /// @brief Return the list of nodes on this Graph.
  ///
  /// @return Return the list of nodes on this Graph.
  std::vector<Node>& nodes() { return nodes_; }

  /// @brief Return the list of nodes on this Graph.
  ///
  /// @return Return the list of nodes on this Graph.
  const std::vector<Node>& nodes() const { return nodes_; }

  /// @brief Return the sorted list of nodes on this Graph.
  ///
  /// @note This function will be removed in future versions of this library.
  ///
  /// @return The sorted list of nodes in this Graph.
  std::vector<Node*>& sorted_nodes() { return sorted_nodes_; }

  /// @brief Return the sorted list of nodes on this Graph.
  ///
  /// @note: This function will be removed in future versions of this library.
  ///
  /// @return The sorted list of nodes in this Graph.
  const std::vector<Node*>& sorted_nodes() const { return sorted_nodes_; }

  /// @brief Returns the memory buffer used to hold all default input buffers.
  ///
  /// @note This is for internal use only.
  ///
  /// @return The memory buffer used to hold all default input buffers.
  MemoryBuffer& input_buffer() { return input_buffer_; }

  /// @brief Returns the memory buffer used to hold all default input buffers.
  ///
  /// @note This is for internal use only.
  ///
  /// @return The memory buffer used to hold all default input buffers.
  const MemoryBuffer& input_buffer() const { return input_buffer_; }

  /// @brief Returns the size that the memory buffer holding output edge data
  ///        will need to be.
  ///
  /// @note This is for internal use only.
  ///
  /// @return The size that the memory buffer holding output edge data will need
  ///         to be.
  size_t output_buffer_size() const { return output_buffer_size_; }

 private:
  // Disallow copying.
  Graph(Graph&);
  Graph& operator=(Graph&);
  Graph(Graph&&);
  Graph& operator=(Graph&&);

  // Utility functions used to sort the graph nodes in FinalizeNodes.
  // Returns true on success, or false if there was an error sorting the graph
  // (such as a circular).
  bool SortGraphNodes();
  bool InsertNode(Node* node);

  const std::string graph_name_;
  std::vector<Node> nodes_;
  std::vector<Node*> sorted_nodes_;
  MemoryBuffer input_buffer_;
  size_t output_buffer_size_;

  bool nodes_finalized_;
};

}  // namespace breadboard

#endif  // BREADBOARD_GRAPH_H_
