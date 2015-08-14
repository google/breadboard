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

#include "event/graph.h"

#include <cstdio>
#include <new>
#include <set>
#include <type_traits>

#include "event/node_interface.h"

namespace fpl {
namespace event {

Graph::~Graph() {
  // Destruct the default values.
  for (auto node = nodes_.begin(); node != nodes_.end(); ++node) {
    const NodeDef* node_def = node->node_def();
    for (size_t i = 0; i < node_def->input_types().size(); ++i) {
      InputEdge& input_edge = node->input_edges()[i];
      if (!input_edge.connected()) {
        // If not connected, it has a default value.
        const Type* type = node_def->input_types()[i];
        uint8_t* ptr = input_buffer_.GetObjectPtr(input_edge.data_offset());
        type->operator_delete_func(ptr);
      }
    }
  }
}

Node* Graph::AddNode(const NodeDef* node_def) {
  assert(!nodes_finalized_);
  nodes_.push_back(Node(node_def));
  return &nodes_.back();
}

bool Graph::InsertNode(Node* node) {
  if (!node->inserted()) {
    node->set_visited(true);
    for (size_t i = 0; i < node->input_edges().size(); ++i) {
      InputEdge& edge = node->input_edges()[i];
      if (edge.connected()) {
        Node& dependency = edge.target().GetTargetNode(&nodes_);
        if (dependency.visited()) {
          // Circular dependency. This is not currently allowed; must be a
          // directed acyclic graph.
          // TODO: Log error properly.
          printf("Graph::InsertNode: Circular dependency\n");
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

bool Graph::FinalizeNodes(EventSystem* event_system) {
  event_system_ = event_system;

  // Make sure each node has the proper number of output edges.
  for (auto node = nodes_.begin(); node != nodes_.end(); ++node) {
    const NodeDef* node_def = node->node_def();
    node->output_edges().resize(node_def->output_types().size());
  }

  // Keep track of the offsets for the input edges and output edges.
  ptrdiff_t current_input_offset = 0;
  ptrdiff_t current_output_offset = 0;

  // Look at each node's input edge so we can mark the output edges that are in
  // use, and so that we know how much memory to allocate for the default
  // values.
  for (auto node = nodes_.begin(); node != nodes_.end(); ++node) {
    const NodeDef* node_def = node->node_def();
    assert(node_def->input_types().size() == node->input_edges().size());
    for (size_t i = 0; i < node_def->input_types().size(); ++i) {
      InputEdge& input_edge = node->input_edges()[i];

      if (input_edge.connected()) {
        // If an input edge is connected to an output edge, mark that edge as
        // connected so we know to allocate memory for it later.
        OutputEdge& target_edge = input_edge.target().GetTargetEdge(&nodes_);
        target_edge.set_connected(true);

      } else {
        // If this is an input with a default value, keep track of where to
        // allocate it when our blob of memory has been allocated.
        const Type* type = node_def->input_types()[i];
        ptrdiff_t data_offset = AdvanceOffset(&current_input_offset, type);
        input_edge.SetDataOffset(data_offset);
      }
    }
  }

  // Now that we know how much space we're going to need, set the buffer size.
  input_buffer_.Initialize(current_input_offset);

  // Construct the default values.
  for (auto node = nodes_.begin(); node != nodes_.end(); ++node) {
    const NodeDef* node_def = node->node_def();
    for (size_t i = 0; i < node_def->input_types().size(); ++i) {
      InputEdge& input_edge = node->input_edges()[i];
      if (!input_edge.connected()) {
        // If not connected, it has a default value.
        const Type* type = node_def->input_types()[i];
        uint8_t* ptr = input_buffer_.GetObjectPtr(input_edge.data_offset());
        type->placement_new_func(ptr);
      }
    }
  }

  // All the default values on the unconnected input nodes has been allocated.
  // Now take care of the output nodes that are connected.
  for (auto node = nodes_.begin(); node != nodes_.end(); ++node) {
    ptrdiff_t node_timestamp_offset =
        AdvanceOffset<Timestamp>(&current_output_offset);
    node->set_timestamp_offset(node_timestamp_offset);

    const NodeDef* node_def = node->node_def();
    for (size_t i = 0; i < node_def->output_types().size(); ++i) {
      OutputEdge& output_edge = node->output_edges()[i];
      if (output_edge.connected()) {
        const Type* type = node_def->output_types()[i];
        ptrdiff_t timestamp_offset =
            AdvanceOffset<Timestamp>(&current_output_offset);
        ptrdiff_t data_offset = AdvanceOffset(&current_output_offset, type);
        output_edge.set_timestamp_offset(timestamp_offset);
        output_edge.set_data_offset(data_offset);
      }
    }
  }

  output_buffer_size_ = current_output_offset;

  if (!SortGraphNodes()) {
    return false;
  }

  nodes_finalized_ = true;
  return true;
}

GraphState::~GraphState() {
  // Destruct the per-graph values.
  if (graph_) {
    for (auto node = graph_->nodes().begin(); node != graph_->nodes().end();
         ++node) {
      const NodeDef* node_def = node->node_def();
      for (size_t i = 0; i < node_def->output_types().size(); ++i) {
        const OutputEdge& output_edge = node->output_edges()[i];
        if (output_edge.connected()) {
          // If connected, it has a per-graph value.
          const Type* type = node_def->output_types()[i];
          uint8_t* ptr = output_buffer_.GetObjectPtr(output_edge.data_offset());
          type->operator_delete_func(ptr);
        }
      }
    }
  }
}

void GraphState::Initialize(Graph* graph) {
  assert(graph->nodes_finalized());
  graph_ = graph;
  output_buffer_.Initialize(graph_->output_buffer_size());
  for (auto node = graph_->nodes().begin(); node != graph_->nodes().end();
       ++node) {
    const NodeDef* node_def = node->node_def();
    for (size_t i = 0; i < node_def->output_types().size(); ++i) {
      const OutputEdge& output_edge = node->output_edges()[i];
      if (output_edge.connected()) {
        const Type* type = node_def->output_types()[i];
        uint8_t* ptr;

        // Initialize timestamp.
        ptr = output_buffer_.GetObjectPtr(output_edge.timestamp_offset());
        new (ptr) Timestamp(0);

        // Initialize object.
        ptr = output_buffer_.GetObjectPtr(output_edge.data_offset());
        type->placement_new_func(ptr);
      }
    }
  }

  for (size_t i = 0; i < graph_->sorted_nodes().size(); ++i) {
    Node* node = graph_->sorted_nodes()[i];
    Inputs in(node, &graph_->nodes(), &graph_->input_buffer(), &output_buffer_);
    node->node_interface()->Initialize(&in);
  }
}

void GraphState::Execute() {
  assert(graph_);
  for (size_t i = 0; i < graph_->sorted_nodes().size(); ++i) {
    Node* node = graph_->sorted_nodes()[i];
    if (IsDirty(*node)) {
      Inputs in(node, &graph_->nodes(), &graph_->input_buffer(),
                &output_buffer_);
      Outputs out(node, &output_buffer_, timestamp_);
      node->node_interface()->Execute(&in, &out);
    }
  }
  ++timestamp_;
}

bool GraphState::IsDirty(const Node& node) const {
  const Timestamp* node_timestamp =
      output_buffer_.GetObject<Timestamp>(node.timestamp_offset());
  if (*node_timestamp == timestamp_) {
    return true;
  }
  for (size_t i = 0; i < node.input_edges().size(); ++i) {
    const InputEdge& input_edge = node.input_edges()[i];
    if (input_edge.connected()) {
      const OutputEdge& output_edge =
          input_edge.target().GetTargetEdge(&graph_->nodes());
      const Timestamp* timestamp =
          output_buffer_.GetObject<Timestamp>(output_edge.timestamp_offset());
      if (*timestamp == timestamp_) {
        return true;
      }
    }
  }
  return false;
}

}  // graph
}  // fpl
