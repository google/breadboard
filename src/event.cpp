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

#include "event/event.h"
#include "event/graph.h"
#include "event/io.h"

namespace fpl {
namespace event {

void NodeEventBroadcaster::RegisterListener(int event_id,
                                            NodeEventListener* listener) {
  auto list_iter = event_listener_lists_.find(event_id);
  if (list_iter == event_listener_lists_.end()) {
    auto insert_result = event_listener_lists_.insert(
        std::make_pair(event_id, ListenerList(&NodeEventListener::node_)));
    list_iter = insert_result.first;
  }
  intrusive_list<NodeEventListener>* listener_list = &list_iter->second;
  // We don't want to insert this node into more than one list, so remove it
  // from whatever list its in before adding it to the new list. This is okay
  // even if we are inserting it into the same list that it's already in.
  listener->node_.remove();
  listener_list->push_back(*listener);
}

void NodeEventBroadcaster::BroadcastEvent(int event_id) {
  auto list_iter = event_listener_lists_.find(event_id);
  if (list_iter != event_listener_lists_.end()) {
    ListenerList& listener_list = list_iter->second;
    for (auto listener_iter = listener_list.begin();
         listener_iter != listener_list.end(); ++listener_iter) {
      listener_iter->base_node_->MarkDirty();
    }
  }
}

}  // event
}  // fpl
