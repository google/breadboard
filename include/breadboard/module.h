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

#ifndef BREADBOARD_MODULE_H_
#define BREADBOARD_MODULE_H_

#include <string>
#include <unordered_map>

#include "breadboard/base_node.h"
#include "breadboard/log.h"
#include "breadboard/node_signature.h"

/// @file breadboard/module.h
///
/// @brief A module represents a collection of nodes with a related purpose.

namespace breadboard {

/// @class Module
///
/// A module is a collection of related NodeSignatures. For example, it may make
/// sense to make a Math module for basic math operations, or an Entity module
/// for entity operations.
class Module {
 public:
  /// @brief Create a Module with the given name.
  explicit Module(const std::string& module_name) : module_name_(module_name) {}

  /// @brief Register a node of type DerivedNode.
  ///
  /// The template argument to this function should be a class that implements
  /// BaseNode. Each BaseNode registered should have one NodeSignature
  /// associated with it. The NodeSignature defines how many input and output
  /// edges and the types of those edges.
  ///
  /// @param[in] node_name The name of the new node.
  ///
  /// @param[in] constructor A function used to construct a new DerivedNode.
  ///
  /// @param[in] destructor A function used to destruct a DerivedNode object.
  template <typename DerivedNode>
  void RegisterNode(const std::string& node_name,
                    const NodeConstructor& constructor,
                    const NodeDestructor& destructor) {
    auto result = signatures_.insert(std::make_pair(
        node_name,
        NodeSignature(&module_name_, node_name, constructor, destructor)));
    NodeDictionary::iterator iter = result.first;

    bool success = result.second;
    if (!success) {
      CallLogFunc(
          "A node named \"%s\" has already been registered in module \"%s\".",
          node_name.c_str(), module_name_.c_str());
      return;
    }
    NodeSignature* signature = &iter->second;
    DerivedNode::OnRegister(signature);
  }

  /// @brief Register a node of type DerivedNode.
  ///
  /// The template argument to this function should be a class that implements
  /// BaseNode. Each BaseNode registered should have one NodeSignature
  /// associated with it. The NodeSignature defines how many input and output
  /// edges and the types of those edges.
  ///
  /// @note This version of the function assumes that DerivedNode needs no
  /// special tear down logic. Instances of DerivedNode are deleted by calling
  /// `delete obj;`. If the type requires any special or tear down consider
  /// using one of the other variations of this function and supply a destructor
  /// callback.
  ///
  /// @param[in] node_name The name of the new node.
  ///
  /// @param[in] constructor A function used to construct a new DerivedNode.
  template <typename DerivedNode>
  void RegisterNode(const std::string& node_name,
                    const NodeConstructor& constructor) {
    return RegisterNode<DerivedNode>(node_name, constructor, DefaultDelete);
  }

  /// @brief Register a node of type DerivedNode.
  ///
  /// The template argument to this function should be a class that implements
  /// BaseNode. Each BaseNode registered should have one NodeSignature
  /// associated with it. The NodeSignature defines how many input and output
  /// edges and the types of those edges.
  ///
  /// @note This version of the function assumes that DerivedNode needs no
  /// special set up or tear down logic. Instances of DerivedNode are created
  /// with a by calling `new DerivedNode()` and deleted by calling `delete
  /// obj;`. If the type requires any special set up or tear down consider using
  /// one of the other variations of this function and supply a constructor
  /// and/or destructor callback.
  ///
  /// @param[in] node_name The name of the new node.
  template <typename DerivedNode>
  void RegisterNode(const std::string& node_name) {
    return RegisterNode<DerivedNode>(node_name, DefaultNew<DerivedNode>);
  }

  /// @brief Returns a pointer to the NodeSignature for a registered node.
  ///
  /// @param[in] node_name The name of the node.
  ///
  /// @return A pointer to the NodeSignature for the given node.
  NodeSignature* GetNodeSignature(const std::string& node_name);

  /// @brief Returns a pointer to the NodeSignature for a registered node.
  ///
  /// @param[in] node_name The name of the node.
  ///
  /// @return A pointer to the NodeSignature for the given node.
  const NodeSignature* GetNodeSignature(const std::string& node_name) const;

 private:
  typedef std::unordered_map<std::string, NodeSignature> NodeDictionary;

  template <typename DerivedNode>
  static BaseNode* DefaultNew() {
    return new DerivedNode();
  }

  static void DefaultDelete(BaseNode* object) { delete object; }

  std::string module_name_;
  NodeDictionary signatures_;
};

}  // namespace breadboard

#endif  // BREADBOARD_MODULE_H_
