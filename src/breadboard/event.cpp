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

void NodeEventListener::MarkDirty() { timestamp_ = graph_state_->timestamp(); }

void NodeEventBroadcaster::RegisterListener(NodeEventListener* listener) {
  auto list_iter = event_listener_lists_.find(listener->event_id());
  if (list_iter == event_listener_lists_.end()) {
    auto insert_result = event_listener_lists_.insert(std::make_pair(
        listener->event_id(), ListenerList(&NodeEventListener::node)));
    list_iter = insert_result.first;
  }

  // We don't want to insert this node into more than one list, so if its in a
  // list we remove it before adding it to the new list. If its already in the
  // list we're going to add it to, we don't need to do anything.
  ListenerList& listener_list = list_iter->second;
  bool in_list = listener->node.in_list();
  if (in_list) {
    bool found = false;
    for (auto listener_iter = listener_list.begin();
         listener_iter != listener_list.end(); ++listener_iter) {
      if (&*listener_iter == listener) {
        found = true;
        break;
      }
    }
    if (!found) {
      listener->node.remove();
      in_list = false;
    }
  }
  if (!in_list) {
    listener_list.push_back(*listener);
  }
}

void NodeEventBroadcaster::BroadcastEvent(EventId event_id) {
  auto list_iter = event_listener_lists_.find(event_id);
  if (list_iter != event_listener_lists_.end()) {
    ListenerList& listener_list = list_iter->second;
    for (auto listener_iter = listener_list.begin();
         listener_iter != listener_list.end(); ++listener_iter) {
      listener_iter->MarkDirty();
      listener_iter->graph_state()->Execute();
    }
  }
}

}  // namespace breadboard
