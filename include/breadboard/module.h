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

namespace breadboard {

// A module is a collection of related NodeSignatures. For example, it may make
// sense to make a Math module for basic math operations, or an Entity module
// for entity operations.
class Module {
 public:
  explicit Module(const std::string& module_name) : module_name_(module_name) {}

  // Add a new NodeSignature. The template argument to this function should be a
  // class that implements BaseNode. Each BaseNode registered should have one
  // NodeSignature associated with it. The NodeSignature defines how many input
  // and output edges and the types of those edges.
  template <typename T>
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
    T::OnRegister(signature);
  }

  template <typename T>
  void RegisterNode(const std::string& name,
                    const NodeConstructor& constructor) {
    return RegisterNode<T>(name, constructor, DefaultDelete);
  }

  template <typename T>
  void RegisterNode(const std::string& name) {
    return RegisterNode<T>(name, DefaultNew<T>);
  }

  NodeSignature* GetNodeSignature(const std::string& name);
  const NodeSignature* GetNodeSignature(const std::string& name) const;

 private:
  typedef std::unordered_map<std::string, NodeSignature> NodeDictionary;

  template <typename T>
  static BaseNode* DefaultNew() {
    return new T();
  }

  static void DefaultDelete(BaseNode* object) { delete object; }

  const std::string module_name_;
  NodeDictionary signatures_;
};

}  // breadboard

#endif  // BREADBOARD_MODULE_H_
