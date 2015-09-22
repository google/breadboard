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

#include <algorithm>

#include "breadboard/event.h"
#include "breadboard/graph_state.h"
#include "breadboard/node_arguments.h"

namespace breadboard {

void NodeEventBroadcaster::RegisterListener(NodeEventListener* listener) {
  auto list_iter = event_listener_lists_.find(listener->event_id());
  if (list_iter == event_listener_lists_.end()) {
    auto insert_result = event_listener_lists_.insert(std::make_pair(
        listener->event_id(), ListenerList(&NodeEventListener::node_)));
    list_iter = insert_result.first;
  }
  fpl::intrusive_list<NodeEventListener>* listener_list = &list_iter->second;
  // We don't want to insert this node into more than one list, so remove it
  // from whatever list its in before adding it to the new list. This is okay
  // even if we are inserting it into the same list that it's already in.
  listener->node_.remove();
  listener_list->push_back(*listener);
}

void NodeEventBroadcaster::BroadcastEvent(EventId event_id) {
  auto list_iter = event_listener_lists_.find(event_id);
  if (list_iter != event_listener_lists_.end()) {
    ListenerList& listener_list = list_iter->second;
    for (auto listener_iter = listener_list.begin();
         listener_iter != listener_list.end(); ++listener_iter) {
      MemoryBuffer* output_buffer =
          listener_iter->graph_state_->output_buffer();
      Timestamp* timestamp = output_buffer->GetObject<Timestamp>(
          listener_iter->target_node_->timestamp_offset());
      *timestamp = listener_iter->graph_state_->timestamp();
    }
  }
}

}  // breadboard
