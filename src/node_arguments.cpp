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

namespace breadboard {

void NodeArguments::VerifyInputEdgeIndex(size_t argument_index) const {
  if (argument_index >= node_->input_edges().size()) {
    CallLogFunc(
        "Attempting to get argument %i when node only has %i input edges.",
        argument_index, static_cast<int>(node_->input_edges().size()));
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

}  // breadboard
