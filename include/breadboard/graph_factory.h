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

#ifndef BREADBOARD_GRAPH_FACTORY_H_
#define BREADBOARD_GRAPH_FACTORY_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "breadboard/graph.h"
#include "breadboard/module_registry.h"

/// @file breadboard/graph_factory.h
///
/// @brief The GraphFactory is a base class that can be used to load Graphs.

namespace breadboard {

/// Different platforms have different ways to load files, so a callback must be
/// supplied to actually load data.
typedef bool (*LoadFileCallback)(const char* filename, std::string* output);

/// @class GraphFactory
///
/// @brief The GraphFactory is a base class that can be used to load Graphs.
///
/// This is a utility class that simplifies the process of loading graphs. It
/// handles loading the file into memory and caching previously loaded files. If
/// the same file is requested multiple times, the cached graph is returned. To
/// use this class ParseData must be overridden to translate the data into nodes
/// and edges, and to parse default values.
class GraphFactory {
 public:
  /// @brief Construct a graph factory using the given modules.
  ///
  /// @param[in] module_registry The ModuleRegistry that contains the set of
  ///            available nodes for the graph.
  /// @param[in] load_file_callback The function to use to load the file from
  ///            disk.
  GraphFactory(ModuleRegistry* module_registry,
               LoadFileCallback load_file_callback)
      : module_registry_(module_registry),
        load_file_callback_(load_file_callback) {}

  /// @brief Destructor for a GraphFactory.
  virtual ~GraphFactory() {}

  /// @brief Load a graph given its filename.
  ///
  /// If this file has already been loaded, a cached copy of the graph is
  /// returned.
  ///
  /// @param[in] filename The name of the file to load.
  ///
  /// @return The loaded Graph.
  Graph* LoadGraph(const char* filename);

 private:
  typedef std::unordered_map<std::string, std::unique_ptr<Graph>> GraphMap;

  /// Parse the data loaded from a file. The data is passed to this function as a
  /// std::string. This function is responsible for filling in the graph (both
  /// the edges and the nodes) using the nodes registered with the
  /// module_registry.
  virtual bool ParseData(ModuleRegistry* module_registry, Graph* graph,
                         const std::string* data) = 0;

  ModuleRegistry* module_registry_;
  LoadFileCallback load_file_callback_;
  GraphMap loaded_graphs_;
};

}  // namespace breadboard

#endif  // BREADBOARD_GRAPH_FACTORY_H_
