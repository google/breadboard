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

#include "breadboard/node_arguments.h"
#include "breadboard/node_signature.h"

namespace breadboard {

// Check to make sure the argument index is in range and the type being
// retrieved is the type expected.
void NodeArguments::VerifyInputPreconditions(size_t argument_index,
                                             const Type* requested_type) const {
  const NodeSignature* signature = node_->signature();
  if (argument_index >= node_->input_edges().size()) {
    CallLogFunc(
        "%s:%s: Attempting to get argument %d when node only has %d input "
        "edges.",
        signature->module_name()->c_str(), signature->node_name().c_str(),
        argument_index, static_cast<int>(node_->input_edges().size()));
    assert(0);
  }
  const Type* expected_type = GetInputEdgeType(node_, argument_index);
  if (requested_type != expected_type) {
    CallLogFunc(
        "%s:%s: Attempting to get input argument %d as type \"%s\" when it "
        "expects type \"%s\".",
        signature->module_name()->c_str(), signature->node_name().c_str(),
        argument_index, requested_type->name, expected_type->name);
    assert(0);
  }
}

// Check to make sure the argument index is in range and the type being set is
// the type expected.
void NodeArguments::VerifyOutputPreconditions(
    size_t argument_index, const Type* requested_type) const {
  const NodeSignature* signature = node_->signature();
  if (argument_index >= node_->output_edges().size()) {
    CallLogFunc(
        "%s:%s: Attempting to get argument %d when node only has %d output "
        "edges.",
        signature->module_name()->c_str(), signature->node_name().c_str(),
        argument_index, static_cast<int>(node_->input_edges().size()));
    assert(0);
  }
  const Type* expected_type = GetOutputEdgeType(node_, argument_index);
  if (requested_type != expected_type) {
    CallLogFunc(
        "%s:%s: Attempting to set output argument %d as type \"%s\" when it "
        "expects type \"%s\".",
        signature->module_name()->c_str(), signature->node_name().c_str(),
        argument_index, requested_type->name, expected_type->name);
    assert(0);
  }
}

bool NodeArguments::IsInputDirty(size_t argument_index) const {
  const InputEdge& input_edge = node_->input_edges()[argument_index];
  if (input_edge.connected()) {
    // If this edge is connected, look at the timestamp on the output edge it's
    // connected to and see if it matches the current timestamp.
    const OutputEdgeTarget& target_edge = input_edge.target();
    const OutputEdge& output_edge = target_edge.GetTargetEdge(nodes_);
    Timestamp* input_edge_timestamp =
        output_memory_->GetObject<Timestamp>(output_edge.timestamp_offset());
    return *input_edge_timestamp == timestamp_;
  } else {
    // If this edge is not connected, it's a default value that never changes
    // and thus is never considered dirty.
    return false;
  }
}

void NodeArguments::VerifyListenerPreconditions(size_t listener_index) const {
  if (listener_index >= node_->listener_offsets().size()) {
    const NodeSignature* signature = node_->signature();
    CallLogFunc(
        "%s:%s: Attempting to get listener %d when node only has %d listeners.",
        signature->module_name()->c_str(), signature->node_name().c_str(),
        listener_index, static_cast<int>(node_->listener_offsets().size()));
    assert(0);
  }
}

bool NodeArguments::IsListenerDirty(size_t listener_index) const {
  VerifyListenerPreconditions(listener_index);

  ptrdiff_t listener_offset = node_->listener_offsets()[listener_index];
  NodeEventListener* listener =
      output_memory_->GetObject<NodeEventListener>(listener_offset);
  return listener->timestamp() == timestamp_;
}

}  // namespace breadboard
