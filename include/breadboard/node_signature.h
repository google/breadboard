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

#ifndef BREADBOARD_NODE_SIGNATURE_H_
#define BREADBOARD_NODE_SIGNATURE_H_

#include <functional>
#include <string>
#include <vector>

#include "breadboard/event.h"
#include "breadboard/type_registry.h"

/// @file breadboard/node_signature.h
///
/// @brief A NodeSignature is a class that represents the various input and
/// output edges of a node, as well as other metadata about the node itself.

namespace breadboard {

class BaseNode;

/// @typedef NodeConstructor
///
/// @brief A typedef for a function (or functor) that constructs a new object
/// that extends BaseNode
typedef std::function<BaseNode*()> NodeConstructor;

/// @typedef NodeDestructor
///
/// @brief A typedef for a function (or functor) that destructs an existing
/// object that extends BaseNode.
typedef std::function<void(BaseNode*)> NodeDestructor;

/// @class NodeSignature
///
/// @brief A NodeSignature is a class that represents the various input and
/// output edges of a node, as well as other metadata about the node itself.
///
/// A NodeSignature defines how many input and ouput edges a Node in a graph is
/// expected to have, and what types those edges are. Additionally,
/// NodeSignatures have a constructor callback which constructs an instance the
/// class that implements BaseNode. The default constructor is used by default,
/// but you may supply your own constructor callback if you need to do any
/// special set up before creating an instance of your class.
///
/// @note You do not need to construct a NodeSignature directly; they are
/// created by the ModuleRegistry when a new node is registered with it.
class NodeSignature {
 public:
  /// @brief Construct a NodeSignature.
  ///
  /// param[in] module_name The name of the module of the node type that this
  /// NodeSignature represents.
  ///
  /// param[in] node_name The name of the node that this NodeSignature
  /// represents.
  ///
  /// param[in] constructor A function that returns a new instance of the node
  /// type that this NodeSignature represents.
  ///
  /// param[in] destructor A function that deletes an existing instance of the
  /// node type that this NodeSignature represents.
  NodeSignature(const std::string* module_name, const std::string& node_name,
                const NodeConstructor& constructor,
                const NodeDestructor& destructor)
      : module_name_(module_name),
        node_name_(node_name),
        constructor_(constructor),
        destructor_(destructor) {}

  /// @brief Returns the name of the module of the node that this NodeSignature
  /// represents.
  ///
  /// @return The module name of the node that this NodeSignature represents.
  const std::string* module_name() const { return module_name_; }

  /// @brief Returns the name of the node that this NodeSignature represents.
  ///
  /// @return The name of the node that this NodeSignature represents.
  const std::string& node_name() const { return node_name_; }

  /// @brief Adds an input edge to a node signature.
  ///
  /// Call this function once for each input edge parameter on the node,
  /// supplying the type of the parameter as the template parameter each
  /// time. Typical usage would look like this:
  ///
  /// ~~~{.cpp}
  ///     class EqualsNode : public BaseNode {
  ///      public:
  ///       static void OnRegister(NodeSignature* node_sig) {
  ///         // Adds two inut arguments of type std::string and one output
  ///         // argument of type bool.
  ///         node_sig->AddInput<std::string>();
  ///         node_sig->AddInput<std::string>();
  ///         node_sig->AddOutput<bool>();
  ///       }
  ///     }
  /// ~~~
  template <typename EdgeType>
  void AddInput() {
    input_types_.push_back(TypeRegistry<EdgeType>::GetType());
  }

  /// @brief Adds an output edge to a node signature.
  ///
  /// Call this function once for each output edge parameter on the node,
  /// supplying the type of the parameter as the template parameter each
  /// time. Typical usage would look like this:
  ///
  /// ~~~{.cpp}
  ///     class EqualsNode : public BaseNode {
  ///      public:
  ///       static void OnRegister(NodeSignature* node_sig) {
  ///         // Adds two inut arguments of type std::string and one output
  ///         // argument of type bool.
  ///         node_sig->AddInput<std::string>();
  ///         node_sig->AddInput<std::string>();
  ///         node_sig->AddOutput<bool>();
  ///       }
  ///     }
  /// ~~~
  template <typename EdgeType>
  void AddOutput() {
    output_types_.push_back(TypeRegistry<EdgeType>::GetType());
  }

  /// @brief Adds a listener to a node signature.
  ///
  /// A node can have any number of listeners associated with it. Each listener
  /// may listen for one and exactly one EventId. A NodeEventListener must be
  /// registered with a NodeEventBroadcaster to recieve events.
  ///
  /// Typical usage would look like this:
  ///
  /// ~~~{.cpp}
  ///     class ExampleNode : public breadboard::BaseNode {
  ///      public:
  ///       static void OnRegister(NodeSignature* signature) {
  ///         signature->AddInput<GameEntity>();
  ///         // The NodeEventListener is created and tracked internally here.
  ///         signature->AddListener(kMyEventId);
  ///       }
  ///     };
  /// ~~~
  ///
  /// @param[in] event_id The event id that this listener should listen for.
  void AddListener(const EventId event_id) {
    event_listeners_.push_back(event_id);
  }

  /// @brief Returns the list of input types.
  ///
  /// @return The list of input types.
  const std::vector<const Type*>& input_types() const { return input_types_; }

  /// @brief Returns the list of output types.
  ///
  /// @return The list of output types.
  const std::vector<const Type*>& output_types() const { return output_types_; }

  /// @brief Returns the list of NodeEventListeners.
  ///
  /// @return The list of NodeEventListeners.
  const std::vector<EventId>& event_listeners() const {
    return event_listeners_;
  }

  /// @brief Constructs a new object of the type that this NodeSignature
  /// represents.
  ///
  /// @note For internal use only.
  ///
  /// @return A new object of the type that this NodeSignature represents.
  BaseNode* Constructor() const;

  /// @brief Destructs an existing object of the type that this NodeSignature
  /// represents.
  ///
  /// @note For internal use only.
  ///
  /// @param[in] base_node The object to be deleted.
  void Destructor(BaseNode* base_node) const;

 private:
  NodeSignature();

  const std::string* module_name_;
  std::string node_name_;
  NodeConstructor constructor_;
  NodeDestructor destructor_;
  std::vector<const Type*> input_types_;
  std::vector<const Type*> output_types_;
  std::vector<EventId> event_listeners_;
};

}  // namespaced breadboard

#endif  // BREADBOARD_NODE_SIGNATURE_H_
