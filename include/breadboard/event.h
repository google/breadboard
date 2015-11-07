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

#ifndef BREADBOARD_EVENT_H_
#define BREADBOARD_EVENT_H_

#include <map>

#include "breadboard/node.h"
#include "fplutil/intrusive_list.h"

/// @file breadboard/event.h
///
/// @brief NodeEventListener and NodeEventBroadcaster are classes that are used
///        to notify GraphState instances when events they care about have
///        occured.

namespace breadboard {

class BaseNode;
class GraphState;

/// @typedef Timestamp
///
/// @brief Timestamp is a type used internally by Breadboard to keep track of
///        which nodes in a graph require a call to Evaluate.
typedef uint32_t Timestamp;

/// @typedef EventId
///
/// @brief An EventID is an opaque pointer that is used as a unique identifier
///        for node events.
typedef const char** EventId;

/// @class NodeEventListener
///
/// @brief A NodeEventListener ensures a node is marked dirty for reevaluation
///        when events it cares about has been broadcast to it.
///
/// NodeEventListeners are registered on the node via the NodeSignature when
/// BaseNode::OnRegister is called. The library user should never need to
/// construct a NodeEventListener manually.
///
/// Typical usage is as follows:
///
/// ~~~{.cpp}
///     class ExampleNode : public breadboard::BaseNode {
///      public:
///       static void OnRegister(NodeSignature* signature) {
///         signature->AddInput<GameEntity>();
///         // The NodeEventListener is created and tracked internally here.
///         signature->AddListener(kMyEventId);
///       }
///       virtual void Initialize(NodeArguments* args) {
///         GameEntity* entity = args->GetInput<GameEntity>(0);
///         // This GameEntity's broadcaster can now broadcast to this node.
///         args->BindBroadcaster(0, &entity->broadcaster);
///       }
///     };
/// ~~~
///
/// If you would like a node to respond to more than one kind of event, you may
/// place multiple listeners on it.
///
/// @note You should never need to instantiate this type directly. Instead you
/// should use NodeSignature::AddListener to create NodeEventListeners.
class NodeEventListener {
 public:
  /// @cond BREADBOARD_INTERNAL
  /// @brief Construct a NodeEventListener for the given GraphState and EventId.
  ///
  /// @param[in] graph_state The GraphState that owns the node that this
  /// NodeEventListener belongs to.
  ///
  /// @param[in] event_id The EventId this listener is listening for.
  NodeEventListener(GraphState* graph_state, EventId event_id)
      : node(), graph_state_(graph_state), timestamp_(0), event_id_(event_id) {}

  /// @brief Returns the EventId this listener is listening for.
  ///
  /// @return The EventId this listener is listening for.
  EventId event_id() const { return event_id_; }

  /// @brief Returns the EventId this listener is listening for.
  ///
  /// @return The EventId this listener is listening for.
  GraphState* graph_state() const { return graph_state_; }

  /// @brief Returns the current Timestamp, used for marking a node as dirty.
  ///
  /// @return The current Timestamp.
  Timestamp timestamp() const { return timestamp_; }

  /// @brief Mark the node this Listener is associated with as dirty.
  void MarkDirty();

  fplutil::intrusive_list_node node;
  /// @endcond

 private:
  GraphState* graph_state_;
  Timestamp timestamp_;
  EventId event_id_;
};

/// @class NodeEventBroadcaster
///
/// @brief A NodeEventBroadcaster is used to notify NodeEventListeners that a
///        specific event has occured so that the appropriate nodes can be
///        marked dirty for re-evaluation.
///
/// The broadcaster keeps different lists internally for each type of event.
/// A NodeEventListener must be registered with a given broadcaster before the
/// listener will be able to respond to events.
///
/// A typical use case would be to add a NodeEventBroadcaster as a field on some
/// kind of game object class. When that object is mutated in a way that a graph
/// node would care about, call BroadcastEvent and specify an event_id. All
/// nodes that are registered to that event_id on the broadcaster will be marked
/// dirty.
///
/// For example, when the game object moves, it would broadcast a kEntityMoved
/// event (where kEntityMoved is an EventId object). All nodes observing that
/// event would then execute the next time the graph itself is executed.
class NodeEventBroadcaster {
 public:
  /// Associate the given listener with this NodeEventBroadcaster and the given
  /// event_id.
  void RegisterListener(NodeEventListener* listener);

  /// For each listener registered with the given event_id on this broadcaster,
  /// mark the node associated with the listener dirty so that it will execute
  /// the next time the graph is executed.
  void BroadcastEvent(EventId event_id);

 private:
  typedef fplutil::intrusive_list<NodeEventListener> ListenerList;

  std::map<EventId, ListenerList> event_listener_lists_;
};

}  // namespace breadboard

/// @def BREADBOARD_DECLARE_EVENT(event_id)
///
/// Whenever you want to declare a new event ID, the preferred way is to use
/// these macros. In you header file you would put:
///
/// ~~~{.cpp}
///     BREADBOARD_DECLARE_EVENT(kMyEventId);
/// ~~~
///
/// And then in your cpp file you would put this:
///
/// ~~~{.cpp}
///     BREADBOARD_DEFINE_EVENT(kMyEventId);
/// ~~~
///
/// Once the event has been declared and defined, you can use it to broadcast
/// events to graphs that are listening for those events
#define BREADBOARD_DECLARE_EVENT(event_id) \
  extern ::breadboard::EventId event_id;

/// @cond BREADBOARD_INTERNAL
/// We need two of each of these macros to ensure that they expand in the
/// right order becuase of quirks in how the C preprocessor works.
/// See http://stackoverflow.com/a/1597129/63791
#define BREADBOARD_STRINGIZE(x) #x
#define BREADBOARD_STRINGIZE2(x) BREADBOARD_STRINGIZE(x)
#define BREADBOARD_CONCAT(x, y) x##y
#define BREADBOARD_CONCAT2(x, y) BREADBOARD_CONCAT(x, y)
/// @endcond

/// @def BREADBOARD_DEFINE_EVENT(event_id)
///
/// Whenever you want to declare a new event ID, the preferred way is to use
/// these macros. In you header file you would put:
///
/// ~~~{.cpp}
///     BREADBOARD_DECLARE_EVENT(kMyEventId);
/// ~~~
///
/// And then in your cpp file you would put this:
///
/// ~~~{.cpp}
///     BREADBOARD_DEFINE_EVENT(kMyEventId);
/// ~~~
///
/// Once the event has been declared and defined, you can use it to broadcast
/// events to graphs that are listening for those events
#define BREADBOARD_DEFINE_EVENT(event_id)                            \
  static const char* BREADBOARD_CONCAT2(BREADBOARD_VAR_, __LINE__) = \
      __FILE__ ":" BREADBOARD_STRINGIZE2(__LINE__);                  \
  ::breadboard::EventId event_id =                                   \
      &BREADBOARD_CONCAT2(BREADBOARD_VAR_, __LINE__);

#endif  // BREADBOARD_EVENT_H_
