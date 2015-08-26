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

#include "event/node.h"
#include "event/node_signature.h"

namespace fpl {
namespace event {

void OutputEdgeTarget::Initialize(unsigned int node_index,
                                  unsigned int edge_index) {
  node_index_ = node_index;
  edge_index_ = edge_index;
}

Node& OutputEdgeTarget::GetTargetNode(std::vector<Node>* nodes) const {
  return (*nodes)[node_index_];
}

const Node& OutputEdgeTarget::GetTargetNode(
    const std::vector<Node>* nodes) const {
  return (*nodes)[node_index_];
}

OutputEdge& OutputEdgeTarget::GetTargetEdge(std::vector<Node>* nodes) const {
  return GetTargetNode(nodes).output_edges()[edge_index_];
}

const OutputEdge& OutputEdgeTarget::GetTargetEdge(
    const std::vector<Node>* nodes) const {
  return GetTargetNode(nodes).output_edges()[edge_index_];
}

Node::Node(const NodeSignature* node_sig)
    : node_sig_(node_sig),
      node_interface_(node_sig->Constructor()),
      input_edges_(),
      output_edges_(),
      inserted_(false),
      visited_(false) {}

}  // event
}  // fpl
