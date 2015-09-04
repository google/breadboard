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

#include "event/base_node.h"
#include "event/graph.h"

namespace fpl {
namespace event {

void BaseNode::MarkDirty() {
  MemoryBuffer* output_buffer = graph_state_->output_buffer();
  Timestamp* timestamp =
      output_buffer->GetObject<Timestamp>(node_->timestamp_offset());
  *timestamp = graph_state_->timestamp();
}

}  // event
}  // fpl
