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

#ifndef FPL_EVENT_GRAPH_LOADER_H_
#define FPL_EVENT_GRAPH_LOADER_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "event/event_system.h"
#include "event/graph.h"

namespace fpl {
namespace event {

// Different platforms have different ways to load files, so a callback must be
// supplied to actually load data.
typedef bool (*LoadFileCallback)(const char* filename, std::string* output);

// This is a utility class that simplifies the process of loading graphs. This
// handles loading the file into memory and caching previously loaded files. If
// the same file is requested multiple times, the cached graph is returned. To
// use this class ParseData must be overridden to translate the data into nodes
// and edges, and to parse default values.
class GraphLoader {
 public:
  GraphLoader(event::EventSystem* event_system,
              LoadFileCallback load_file_callback)
      : event_system_(event_system), load_file_callback_(load_file_callback) {}

  // Load the graph given it's filename. If this file has already been loaded, a
  // cached copy of the graph is returned.
  Graph* LoadGraph(const char* filename);

 private:
  typedef std::unordered_map<std::string, std::unique_ptr<event::Graph>>
      GraphMap;

  // Parse the data loaded from a file. The data is passed to this function as a
  // std::string. This function is responsible for filling in the graph (both
  // the edges and the nodes) using the nodes registered with the event_system.
  virtual bool ParseData(event::EventSystem* event_system, event::Graph* graph,
                         const std::string* data) = 0;

  event::EventSystem* event_system_;
  LoadFileCallback load_file_callback_;
  GraphMap loaded_graphs_;
};

}  // event
}  // fpl

#endif  // FPL_EVENT_GRAPH_LOADER_H_
