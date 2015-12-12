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

#include "breadboard/graph.h"

#include <new>
#include <set>
#include <type_traits>

#include "breadboard/base_node.h"
#include "breadboard/log.h"

namespace breadboard {

Graph::~Graph() {
  // Destruct the default values.
  for (auto node = nodes_.begin(); node != nodes_.end(); ++node) {
    const NodeSignature* signature = node->signature();
    for (size_t i = 0; i < signature->input_parameters().size(); ++i) {
      InputEdge& input_edge = node->input_edges()[i];
      if (!input_edge.connected()) {
        // If not connected, it has a default value.
        const Type* type = signature->input_parameters()[i].type;
        // Only do this on non-void objects. Attempting to access a void edge
        // can be troublesome in the case where the last edge listed is of type
        // void.
        if (type->size > 0) {
          uint8_t* ptr = input_buffer_.GetObjectPtr(input_edge.data_offset());
          type->operator_delete_func(ptr);
        }
      }
    }
  }
}

Node* Graph::AddNode(const NodeSignature* signature) {
  assert(!nodes_finalized_);
  nodes_.push_back(Node(signature));
  return &nodes_.back();
}

// Returns the index of a node in the graph. This takes linear time, but we
// don't care that much becuase this is only called when there is an error.
static int NodeIndex(const std::vector<Node>& nodes, const Node& node) {
  for (size_t i = 0; i < nodes.size(); ++i) {
    if (&nodes[i] == &node) {
      return static_cast<int>(i);
    }
  }
  // Should not ever reach this point: that would mean we're looking for a node
  // in a different graph.
  assert(0);
  return -1;
}

bool Graph::InsertNode(Node* node) {
  if (!node->inserted()) {
    node->set_visited(true);
    for (size_t i = 0; i < node->input_edges().size(); ++i) {
      InputEdge& edge = node->input_edges()[i];
      if (edge.connected()) {
        Node& dependency = edge.target().GetTargetNode(&nodes_);
        const Type* input_type = node->signature()->input_parameters()[i].type;
        const Type* output_type =
            dependency.signature()
                ->output_parameters()[edge.target().edge_index()]
                .type;
        if (input_type != output_type) {
          int input_node_index = NodeIndex(nodes_, *node);
          CallLogFunc(
              "Could not resolve graph \"%s\": Type mismatch. Node %d, input "
              "edge %d is type \"%s\" but is connected to node %d, output edge "
              "%d of type \"%s\".",
              graph_name_.c_str(), input_node_index, i, input_type->name,
              edge.target().node_index(), edge.target().edge_index(),
              output_type->name);
          return false;
        } else if (dependency.visited()) {
          // Circular dependency. This is not currently allowed; must be a
          // directed acyclic graph.
          CallLogFunc("Could not resolve graph: Circular dependency.");
          return false;
        } else if (!dependency.inserted()) {
          if (!InsertNode(&dependency)) {
            // One of the errors above was hit in a recursive call, error out.
            return false;
          }
        }
      }
    }
    node->set_visited(false);
    node->set_inserted(true);
    sorted_nodes_.push_back(node);
  }
  return true;
}

// The nodes form a Directed Acyclic Graph. We sort them to produce a list that
// is guaranteed to be ordered such that each node with a dependency will come
// after the node it depends on. The algorithm is described here:
//
//     https://en.wikipedia.org/wiki/Topological_sorting
bool Graph::SortGraphNodes() {
  for (size_t i = 0; i != nodes_.size(); ++i) {
    Node& node = nodes_[i];
    if (!InsertNode(&node)) {
      // Error in evaluation; report error.
      return false;
    }
  }
  return true;
}

static ptrdiff_t Align(ptrdiff_t ptr, size_t alignment) {
  // Alignment must be a power of 2.
  assert((alignment & (alignment - 1)) == 0);
  return (ptr + alignment - 1) & ~(alignment - 1);
}

static ptrdiff_t AdvanceOffset(ptrdiff_t* offset, size_t size,
                               size_t alignment) {
  ptrdiff_t result = Align(*offset, alignment);
  *offset = result + size;
  return result;
}

static ptrdiff_t AdvanceOffset(ptrdiff_t* offset, const Type* type) {
  return AdvanceOffset(offset, type->size, type->alignment);
}

template <typename T>
static ptrdiff_t AdvanceOffset(ptrdiff_t* offset) {
  return AdvanceOffset(offset, sizeof(T), std::alignment_of<T>::value);
}

bool Graph::FinalizeNodes() {
  // Make sure each node has the proper number of output edges.
  for (auto node = nodes_.begin(); node != nodes_.end(); ++node) {
    const NodeSignature* signature = node->signature();
    node->output_edges().resize(signature->output_parameters().size());
  }

  // Keep track of the offsets for the input edges and output edges.
  ptrdiff_t current_input_offset = 0;
  ptrdiff_t current_output_offset = 0;

  // Look at each node's input edge so we can mark the output edges that are in
  // use, and so that we know how much memory to allocate for the default
  // values.
  for (size_t i = 0; i < nodes_.size(); ++i) {
    Node* node = &nodes_[i];
    const NodeSignature* signature = node->signature();
    if (signature->input_parameters().size() != node->input_edges().size()) {
      CallLogFunc(
          "Error in graph \"%s\": Node %d got %d edges, but expected %d",
          graph_name_.c_str(), i, node->input_edges().size(),
          signature->input_parameters().size());
      return false;
    }
    for (size_t j = 0; j < signature->input_parameters().size(); ++j) {
      InputEdge& input_edge = node->input_edges()[j];

      if (input_edge.connected()) {
        // If an input edge is connected to an output edge, mark that edge as
        // connected so we know to allocate memory for it later.
        OutputEdge& target_edge = input_edge.target().GetTargetEdge(&nodes_);
        target_edge.set_connected(true);

      } else {
        // If this is an input with a default value, keep track of where to
        // allocate it when our blob of memory has been allocated.
        const Type* type = signature->input_parameters()[j].type;
        ptrdiff_t data_offset = AdvanceOffset(&current_input_offset, type);
        input_edge.SetDataOffset(data_offset);
      }
    }
  }

  // Now that we know how much space we're going to need, set the buffer size.
  input_buffer_.Initialize(current_input_offset);

  // Construct the default values.
  for (auto node = nodes_.begin(); node != nodes_.end(); ++node) {
    const NodeSignature* signature = node->signature();
    for (size_t i = 0; i < signature->input_parameters().size(); ++i) {
      InputEdge& input_edge = node->input_edges()[i];
      if (!input_edge.connected()) {
        // If not connected, it has a default value.
        const Type* type = signature->input_parameters()[i].type;
        assert(type);
        if (type->size > 0) {
          uint8_t* ptr = input_buffer_.GetObjectPtr(input_edge.data_offset());
          type->placement_new_func(ptr);
        }
      }
    }
  }

  // All the default values on the unconnected input nodes has been allocated.
  // Now take care of the output nodes that are connected.
  for (auto node = nodes_.begin(); node != nodes_.end(); ++node) {
    ptrdiff_t node_timestamp_offset =
        AdvanceOffset<Timestamp>(&current_output_offset);
    node->set_timestamp_offset(node_timestamp_offset);

    const NodeSignature* signature = node->signature();
    for (size_t i = 0; i < signature->output_parameters().size(); ++i) {
      OutputEdge& output_edge = node->output_edges()[i];
      if (output_edge.connected()) {
        const Type* type = signature->output_parameters()[i].type;
        ptrdiff_t timestamp_offset =
            AdvanceOffset<Timestamp>(&current_output_offset);
        ptrdiff_t data_offset = AdvanceOffset(&current_output_offset, type);
        output_edge.set_timestamp_offset(timestamp_offset);
        output_edge.set_data_offset(data_offset);
      }
    }

    // Initialize the listener offsets.
    for (size_t i = 0; i < signature->event_listeners().size(); ++i) {
      ptrdiff_t listener_offset =
          AdvanceOffset<NodeEventListener>(&current_output_offset);
      node->listener_offsets().push_back(listener_offset);
    }
  }

  output_buffer_size_ = current_output_offset;

  if (!SortGraphNodes()) {
    return false;
  }

  nodes_finalized_ = true;
  return true;
}

}  // namespace breadboard
