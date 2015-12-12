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

#include <cstddef>

#include "breadboard/node.h"
#include "breadboard/node_signature.h"

namespace breadboard {

void OutputEdgeTarget::Initialize(unsigned int node_index,
                                  unsigned int edge_index) {
  node_index_ = node_index;
  edge_index_ = edge_index;
}

Node& OutputEdgeTarget::GetTargetNode(std::vector<Node>* nodes) const {
  assert(node_index_ < nodes->size());
  return (*nodes)[node_index_];
}

const Node& OutputEdgeTarget::GetTargetNode(
    const std::vector<Node>* nodes) const {
  assert(node_index_ < nodes->size());
  return (*nodes)[node_index_];
}

OutputEdge& OutputEdgeTarget::GetTargetEdge(std::vector<Node>* nodes) const {
  return GetTargetNode(nodes).output_edges()[edge_index_];
}

const OutputEdge& OutputEdgeTarget::GetTargetEdge(
    const std::vector<Node>* nodes) const {
  return GetTargetNode(nodes).output_edges()[edge_index_];
}

Node::Node(const NodeSignature* signature)
    : signature_(signature),
      base_node_(signature->Constructor()),
      input_edges_(),
      output_edges_(),
      timestamp_offset_(0),
      inserted_(false),
      visited_(false) {}

const Type* GetInputEdgeType(const Node* node, std::size_t index) {
  return node->signature()->input_parameters()[index].type;
}

const Type* GetOutputEdgeType(const Node* node, std::size_t index) {
  return node->signature()->output_parameters()[index].type;
}

}  // namespace breadboard
