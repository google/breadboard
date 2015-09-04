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

#ifndef FPL_EVENT_EVENT_H_
#define FPL_EVENT_EVENT_H_

#include <map>

#include "fplutil/intrusive_list.h"

namespace fpl {
namespace event {

class BaseNode;

// A NodeEventListener is used to mark graph nodes dirty when a
// NodeEventBroadcaster nofifies them of an event. NodeEventListeners should be
// added as a member of the class implementing the BaseNode. For example, you
// may have a node that should be marked dirty when the player moves You could
// set up a node as so:
//
// class ExampleNode : public event::BaseNode {
//  public:
//   ExampleNode() : listener_(this) {}
//
//   ...
//
//  private:
//   event::NodeEventListener listener_;
// };
//
// If you would like a node to respond to more than one kind of event, you may
// place multiple listeners on it.
class NodeEventListener {
 public:
  NodeEventListener(BaseNode* base_node)
      : node_(), base_node_(base_node) {}

 private:
  friend class NodeEventBroadcaster;
  intrusive_list_node node_;
  BaseNode* base_node_;
};

// A NodeEventBroadcaster is used to notify NodeEventListeners that a specific
// event has occured so that the appropriate nodes can be marked dirty for
// re-evaluation. The broadcaster keeps different lists for each type of events.
// NodeEventListeners need to be registered with a given broadcaster before the
// listener will do anything.
//
// A typical use case would be to add a NodeEventBroadcaster as a field on some
// kind of game object class. When that object is mutated in a way that a graph
// node would care about, call BroadcastEvent and specify an event_id. All nodes
// that are registered on that event_id on the broadcaster will be marked dirty.
//
// For example, when the game object moves, it would broadcast an kEntityMoved
// event (where kEntityMoved is an enum value). All nodes observing that event
// would then execute the next time the graph itself is executed.
struct NodeEventBroadcaster {
 public:
  // Associate the given listener with this NodeEventBroadcaster and the given
  // event_id.
  void RegisterListener(int event_id, NodeEventListener* listener);

  // For each listener registered with the given event_id on this broadcaster,
  // mark the node associated with the listener dirty so that it will execute
  // the next time the graph is executed.
  void BroadcastEvent(int event_id);

 private:
  typedef intrusive_list<NodeEventListener> ListenerList;

  std::map<int, ListenerList> event_listener_lists_;
};

}  // event
}  // fpl

#endif  // FPL_EVENT_EVENT_H_
