Events    {#breadboard_guide_events}
======

## Overview

Events are what drive Graph execution. Most nodes will have some set of inputs
and outputs, but without getting updates from the game state that exists outside
the graph, the graph nodes will never execute.

To broadcast events to nodes in a graph, two structures are needed, a
NodeEventListener and a NodeEventBroadcaster.

## NodeEventBroadcaster

A NodeEventBroadcaster is an object that can be included on any game object that
makes sense for your game. They can exist on world objects, players, entities,
or anything else. Typically a specific Broadcaster will be associated with the
object that holds it. That way you can do things like have an event that
triggers when your entity collides with something. In that case, you would
broadcast a collision event from the broadcaster which that specific entity
owns. That event would then be broadcast to only the graphs that are listening
to that broadcaster.

A broadcaster can be registered to any number of listeners, and can broadcast
any kind of event.

Events may be broadcast by calling
`NodeEventBroadcaster::BroadcastEvent(event_id)`.

## NodeEventListener

NodeEventListeners exist on the Node and are not instantiated directly. Instead,
during the OnRegistration step, as many listeners as needed may be added to your
node. For example:

~~~{.cpp}
    // ...
    static void OnRegister(NodeSignature* signature) {
      signature->AddInput<GameActor>();
      signature->AddListener(kCollisionEventId);
    }
    virtual void Initialize(NodeArguments* args) {
      GameActor* actor = args->GetInput<GameActor>(0);
      args->BindBroadcaster(0, actor->broadcaster);
    }
    // ...
~~~

In this example this node registers a single NodeEventListener that is listening
for the event kCollisionEventId. During initialization, the actor associated
with this node has it's broadcaster bound to that listener. Now any time that
the actor fires a kCollisionEventId, this node will be marked dirty and have its
Execute function (not shown here) called.
