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

#ifndef BREADBOARD_BASE_NODE_H_
#define BREADBOARD_BASE_NODE_H_

#include <cassert>

#include "breadboard/event.h"
#include "breadboard/node_arguments.h"
#include "breadboard/node_signature.h"

/// @file breadboard/base_node.h
///
/// @brief A BaseNode is the common class that all nodes that are to be used in
///        a graph must extend.

namespace breadboard {

/// @class BaseNode
///
/// @brief A BaseNode is the common class that all nodes that are to be used in
/// a graph must extend.
///
/// Each class that implements BaseNode must be registered in a Module before it
/// may be used in a graph.
///
/// Nodes are the primary method of performing actions in Breadboard. Nodes can
/// take any number of input edge parameters and perform actions based on those
/// parameters. They in turn provide various output arguments which may be
/// consumed by other nodes. The connnections between nodes can be used to
/// script complex behavior.
class BaseNode {
 public:
  /// @brief Destructor for a BaseNode.
  virtual ~BaseNode() {}

  /// @brief When this node is registered, the input edges, output edges and
  /// active listeners for this node are defined by this function.
  ///
  /// Although this is a static function, all classes inheriting from BaseNode
  /// must implement an OnRegister function. This function is used to define the
  /// inputs, outputs, and listeners that this node requires.
  ///
  /// @param[in,out] signature An object that represents the input edge
  ///                          parameters, output edge parameters, and active
  ///                          listeners for this kind of node.
  static void OnRegister(NodeSignature* signature) {
    (void)signature;
    assert(false); // This function should be implemented in the derived class
  }

  /// @brief Initialize is called once upon when an instance of a GraphState is
  /// initiazlized. This function can be used to set output edge arguments that
  /// are guaranteed to never change or do other one-time setup.
  ///
  /// Initialize is called once when a GraphState object is being initialized.
  /// You can use this to do any special set up. For example, if this is a node
  /// that has no inputs and only outputs that are unchanging, you can set them
  /// once here and not need to worry about doing anything in Execute. This is
  /// also a useful place to bind listeners with broadcasters.
  virtual void Initialize(NodeArguments* args) { (void)args; }

  /// @brief Execute is called any time the node is marked dirty in the graph.
  ///
  /// Execute is called any time this node in the graph is marked dirty. This
  /// can happen in two ways. One way is that this node has a listener that has
  /// had an event broadcast to it. The other is that another node updated an
  /// output edge that this node is observing.
  ///
  /// This function is where the bulk of the interesting logic should occur in a
  /// node.
  virtual void Execute(NodeArguments* args) { (void)args; }
};

}  // namespace breadboard

#endif  // BREADBOARD_BASE_NODE_H_
