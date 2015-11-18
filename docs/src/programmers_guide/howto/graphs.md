Graphs    {#breadboard_guide_graphs}
======

## Overview

Graphs are split up into two main classes, the Graph and GraphState. The Graph
class defines the set of nodes that make up the Graph, as well as the
connections between them and any default values. You can think of the Graph as
the immutable definition of the set of behavior. The GraphState is a stateful
object backed by a Graph. You can have any number of instances of GraphStates
that all point at the same Graph. This is useful for when you have, for example,
many duplicates of the same kind of actor in your game.

### Limitations

In the initial release of Breadboard, Graphs are implemented such that they may
not contain cycles. That is, if node A has an input edge pointing at node B,
then node B can not have an input edge pointing at node A. This restriction may
be lifted in a future update.
