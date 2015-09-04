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

#ifndef FPL_EVENT_BASE_NODE_H_
#define FPL_EVENT_BASE_NODE_H_

#include "event/event.h"
#include "event/io.h"
#include "event/node_signature.h"

namespace fpl {
namespace event {

class GraphState;

// A node is an abstract base class for all nodes in a graph. Each class that
// implements BaseNode must be registered in a Module before it may be used in a
// graph, and by registering in the Module it will have a NodeDef which
// specifies the number of expected Inputs and Outputs that will be passed to
// it.
//
// A node must have an Execute function which is to be called when any of it's
// dependencies update.
//
// They may also specify an initialize function for one time initialization when
// they are created.
class BaseNode {
 public:
  virtual ~BaseNode() {}

  // All classes inheriting from BaseNode must imlement the static Register
  // function shown here. Because this is a static function it cannot be
  // virtual.
  //
  // static void OnRegister(NodeSignature* node_sig);

  // Initialize is called once when a GraphState object is being initialized.
  // You can use this to do any special set up. For example, if this is a node
  // that has no inputs and only outputs that are unchanging, you can set them
  // once here and not need to worry about doing anything in Execute.
  virtual void Initialize(Inputs* inputs, Outputs* outputs) {
    (void)inputs;
    (void)outputs;
  }

  // Execute is called any time the GraphState Execute function is called. Only
  // Nodes that are marked dirty or have had one of their inputs change will be
  // run. If there has been no changes to any of a nodes inputs, the call to
  // Execute on this node will be skipped.
  virtual void Execute(Inputs* inputs, Outputs* outputs) {
    (void)inputs;
    (void)outputs;
  }

  // Marks this node to dirty so that the next time the graph is executed this
  // node will have its Execute method called. This function has no effect when
  // called from within Execute.
  void MarkDirty();

 private:
  friend class GraphState;

  void InitializeInternal(Node* node, GraphState* graph_state) {
    node_ = node;
    graph_state_ = graph_state;
  }

  // The graph and node associated with this BaseNode. This is needed to mark
  // the node dirty when events occur.
  Node* node_;
  GraphState* graph_state_;
};

}  // event
}  // fpl

#endif  // FPL_EVENT_BASE_NODE_H_
