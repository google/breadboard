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

#include "breadboard/graph_state.h"

#include <new>
#include <set>
#include <type_traits>

#include "breadboard/base_node.h"

namespace breadboard {

GraphState::~GraphState() {
  // Destruct the per-graph values.
  if (graph_) {
    for (auto node = graph_->nodes().begin(); node != graph_->nodes().end();
         ++node) {
      const NodeSignature* signature = node->signature();
      for (size_t i = 0; i < signature->output_parameters().size(); ++i) {
        const OutputEdge& output_edge = node->output_edges()[i];
        if (output_edge.connected()) {
          // If connected, it has a per-graph value.
          const Type* type = signature->output_parameters()[i].type;

          // Destruct timestamp.
          Timestamp* timestamp = output_buffer_.GetObject<Timestamp>(
              output_edge.timestamp_offset());
          timestamp->~Timestamp();
          (void)timestamp;  // Avoid erroneous "not referenced" warning in VS.

          // Destruct object.
          // Only do this on non-void objects. Attempting to access a void edge
          // can be troublesome in the case where the last edge listed is of
          // type void.
          if (type->size > 0) {
            uint8_t* ptr =
                output_buffer_.GetObjectPtr(output_edge.data_offset());
            type->operator_delete_func(ptr);
          }
        }
      }
      for (size_t i = 0; i < node->listener_offsets().size(); ++i) {
        ptrdiff_t listener_offset = node->listener_offsets()[i];
        NodeEventListener* listener =
            output_buffer_.GetObject<NodeEventListener>(listener_offset);
        listener->~NodeEventListener();
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
    uint8_t* ptr;
    const NodeSignature* signature = node->signature();

    // Initialize the memory for the output edges.
    for (size_t i = 0; i < signature->output_parameters().size(); ++i) {
      const OutputEdge& output_edge = node->output_edges()[i];
      if (output_edge.connected()) {
        const Type* type = signature->output_parameters()[i].type;

        // Initialize timestamp.
        ptr = output_buffer_.GetObjectPtr(output_edge.timestamp_offset());
        new (ptr) Timestamp(0);

        // Initialize object.
        // Only do this on non-void objects. Attempting to access a void edge
        // can be troublesome in the case where the last edge listed is of
        // type void.
        if (type->size > 0) {
          ptr = output_buffer_.GetObjectPtr(output_edge.data_offset());
          type->placement_new_func(ptr);
        }
      }
    }

    // Initialize the memory for the listeners.
    for (size_t i = 0; i < signature->event_listeners().size(); ++i) {
      EventId event_id = signature->event_listeners()[i].event_id;
      ptr = output_buffer_.GetObjectPtr(node->listener_offsets()[i]);
      new (ptr) NodeEventListener(this, event_id);
    }
  }

  for (size_t i = 0; i < graph_->sorted_nodes().size(); ++i) {
    Node* node = graph_->sorted_nodes()[i];
    NodeArguments args(node, &graph_->nodes(), &graph_->input_buffer(),
                       &output_buffer_, timestamp_);
    node->base_node()->Initialize(&args);
  }
  ++timestamp_;
}

void GraphState::Execute() {
  assert(graph_);
  for (size_t i = 0; i < graph_->sorted_nodes().size(); ++i) {
    Node* node = graph_->sorted_nodes()[i];
    if (IsDirty(*node)) {
      NodeArguments args(node, &graph_->nodes(), &graph_->input_buffer(),
                         &output_buffer_, timestamp_);
      node->base_node()->Execute(&args);
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
  for (size_t i = 0; i < node.listener_offsets().size(); ++i) {
    const NodeEventListener* listener =
        output_buffer_.GetObject<NodeEventListener>(node.listener_offsets()[i]);
    if (listener->timestamp() == timestamp_) {
      return true;
    }
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

}  // namespace breadboard
