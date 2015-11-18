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
//
#include <stdio.h>

#include <sstream>
#include <string>

#include "breadboard/base_node.h"
#include "breadboard/graph_state.h"
#include "breadboard/module_registry.h"
#include "breadboard/modules/common.h"
#include "fplutil/main.h"
#include "fplutil/print.h"


// This is a sample that counts an event occurrence.
//
// It demonstrates usage of:
// - breadboard::Node to create and configure nodes.
// - breadboard::Graph to build a graph of event counter.
// - breadboard::NodeEventBroadcaster && breadboard::NodeEventListener.
// - inter-operations of the above graph elements
namespace sample {
using namespace breadboard;
const char *kCounterEventName = "graph-sample-event";
const char *kModuleName = "sample_module";
const char *kCaptureNodeName = "capture_event";
const char *kCountNodeName = "count_event";
const char *kPrintNodeName = "print_event";
const EventId kCounterEvent = &kCounterEventName;

// CaptureEvent Node:
//    No Input; 1 Integer Output(increment value to add into counter)
//    Listen to kCounterEvent. Event binding happens at node creation time
class CaptureEvent : public BaseNode {
 public:
  CaptureEvent(breadboard::NodeEventBroadcaster *broadcaster)
      : broadcaster_(broadcaster) {}
  virtual ~CaptureEvent() {}

  static void OnRegister(NodeSignature *node_sig) {
    node_sig->AddOutput<int>();
    node_sig->AddListener(kCounterEvent);
  }

  virtual void Initialize(NodeArguments *args) {
    args->BindBroadcaster(0, broadcaster_);
  }

  // Send an incremental value to next node for each event
  virtual void Execute(NodeArguments *args) { args->SetOutput(0, 1); }

 private:
  breadboard::NodeEventBroadcaster *broadcaster_;
};

// CountEvent Node:
//    One integer input, one std::string output
//    Accept input and add it into internal counter,
//    pass counter value to output as std::string
class CountEvent : public BaseNode {
 public:
  CountEvent() : count_(0) {}
  virtual ~CountEvent() {}

  static void OnRegister(NodeSignature *node_sig) {
    node_sig->AddInput<int>();
    node_sig->AddOutput<std::string>();
  }

  virtual void Execute(NodeArguments *args) {
    count_ += *args->GetInput<int>(0);
    std::stringstream ss;
    ss << count_;
    args->SetOutput(0, ss.str());
  }

 private:
  int count_;
};

// PrintEvent Node:
//   One std::string input, no output
//   Print string from input to debug window
class PrintEvent : public BaseNode {
 public:
  virtual ~PrintEvent() {}

  static void OnRegister(NodeSignature *node_sig) {
    node_sig->AddInput<std::string>();
  }

  virtual void Execute(NodeArguments *args) {
    auto str = args->GetInput<std::string>(0);
    CallLogFunc("Event Counter = %s\n", str->c_str());
  }
};

// Build up our node reservoir into a sample module (kModuleName)
// then it could be used to create a graph
void InitializeSampleModule(ModuleRegistry *module_registry,
                            breadboard::NodeEventBroadcaster *broadcaster) {
  // Register the types we need for our module
  TypeRegistry<int>::RegisterType("Int");
  TypeRegistry<std::string>::RegisterType("String");

  auto capture_event_ctor = [broadcaster]() {
    return new CaptureEvent(broadcaster);
  };

  Module *module = module_registry->RegisterModule(kModuleName);
  module->RegisterNode<CaptureEvent>(kCaptureNodeName, capture_event_ctor);
  module->RegisterNode<CountEvent>(kCountNodeName);
  module->RegisterNode<PrintEvent>(kPrintNodeName);
}

// CreateGraph():
//    Build a simple graph of 3 nodes: CaptureEvent -> CountEvent -> PrintEvent
breadboard::Graph *CreateGraph(const breadboard::ModuleRegistry *mod_registry) {
  auto graph = new breadboard::Graph("graph_sample");
  if (!graph) {
    return nullptr;
  }

  // create node index 0: event catcher node
  auto module = mod_registry->GetModule(kModuleName);
  auto node_sig = module->GetNodeSignature(kCaptureNodeName);
  auto node = graph->AddNode(node_sig);

  // create node index 1: event counter node
  // connect the only input to node 0's output 0
  node_sig = module->GetNodeSignature(kCountNodeName);
  node = graph->AddNode(node_sig);
  node->input_edges().push_back(breadboard::InputEdge());
  node->input_edges().back().SetTarget(0, 0);

  // create node index 2: event printer node
  // connect the only input to node 1's output 0
  node_sig = module->GetNodeSignature(kPrintNodeName);
  node = graph->AddNode(node_sig);
  node->input_edges().push_back(breadboard::InputEdge());
  node->input_edges().back().SetTarget(1, 0);

  // validate our graph
  if (!graph->FinalizeNodes()) {
    delete graph;
    graph = nullptr;
  }

  return graph;
}

// a helper function to support logging for breadboard
void BreadboardLogFunc(const char *fmt, va_list args) { vprintf(fmt, args); }

} // namespace sample

extern "C" int main(int /*argc*/, char **argv) {
  // declare our event broadcaster, needed for node creation
  breadboard::NodeEventBroadcaster broadcaster;

  breadboard::ModuleRegistry module_registry;
  breadboard::RegisterLogFunc(sample::BreadboardLogFunc);
  sample::InitializeSampleModule(&module_registry, &broadcaster);

  auto graph = sample::CreateGraph(&module_registry);
  if (!graph) {
    printf("Failed to build graph\n");
    return -1;
  }

  {
    breadboard::GraphState graph_state;
    graph_state.Initialize(graph);

    breadboard::NodeEventListener listener(&graph_state, sample::kCounterEvent);
    broadcaster.RegisterListener(&listener);

    int loop = 0;
    while (loop++ < 500) {
      broadcaster.BroadcastEvent(sample::kCounterEvent);
    }
  }

  delete graph;
  printf("Success: %s completed\n", argv[0]);

  return 0;
}
