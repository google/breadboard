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

#include "event/node.h"
#include "fplutil/intrusive_list.h"

namespace fpl {
namespace event {

class BaseNode;
class GraphState;

typedef const char** EventId;

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
  NodeEventListener(GraphState* graph_state, Node* target_node,
                    EventId event_id)
      : node_(),
        graph_state_(graph_state),
        target_node_(target_node),
        event_id_(event_id) {}

  EventId event_id() const { return event_id_; }

 private:
  friend class NodeEventBroadcaster;

  intrusive_list_node node_;
  GraphState* graph_state_;
  Node* target_node_;
  EventId event_id_;
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
class NodeEventBroadcaster {
 public:
  // Associate the given listener with this NodeEventBroadcaster and the given
  // event_id.
  void RegisterListener(NodeEventListener* listener);

  // For each listener registered with the given event_id on this broadcaster,
  // mark the node associated with the listener dirty so that it will execute
  // the next time the graph is executed.
  void BroadcastEvent(EventId event_id);

 private:
  typedef intrusive_list<NodeEventListener> ListenerList;

  std::map<EventId, ListenerList> event_listener_lists_;
};

}  // event
}  // fpl

// Whenever you want to declare a new event ID, the preferred way is to use
// these macros. In you header file you would put:
//
//     FPL_EVENT_DECLARE_EVENT(kMyEventId);
//
// And then in your cpp file you would put this:
//
//     FPL_EVENT_DEFINE_EVENT(kMyEventId);
//
// Once the event has been declared and defined, you can use it to broadcast
// events to graphs that are listening for those events
#define FPL_EVENT_DECLARE_EVENT(event_id) extern ::fpl::event::EventId event_id;

#define FPL_EVENT_CONCAT(a, b) a##b

#define FPL_EVENT_DEFINE_EVENT_INTERNAL(event_id, str)            \
  static const char* FPL_EVENT_CONCAT(FPL_EVENT_VAR, __LINE__) = #str; \
  ::fpl::event::EventId event_id = &FPL_EVENT_CONCAT(FPL_EVENT_VAR, __LINE__);

#define FPL_EVENT_DEFINE_EVENT(event_id)    \
  FPL_EVENT_DEFINE_EVENT_INTERNAL(event_id, \
                                  FPL_EVENT_CONCAT(#event_id, __LINE__))

#endif  // FPL_EVENT_EVENT_H_
