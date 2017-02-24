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

/// This is a default implementation of the virtual functions in GraphFactory.
/// If you use the default names for the component FlatBuffers schemas in your
/// project, you can use this as your entity factory by creating a source file,
/// including your generated FlatBuffers headers, and then including this file.
///
/// This default graph factory assumes the following 4 generated FlatBuffers
/// types defined for serialization of component data:
///
/// * InputType
///     The union of the edge types you are using in your game (e.g. Bool, Int,
///     String, Entity).
/// * InputEdgeDef
///     A table containing one instance of the above union.
/// * NodeDef
///     A table containing a module name, a node name, and a list of
///     InputEdgeDefs, defining the default values of the input edges.
/// * GraphDef
///     A list of NodeDefs.
///
/// These types should be defined in your FlatBuffers schema as follows:
///
/// namespace mygame;
/// union InputType {
///   breadboard.module_library.OutputEdgeTarget,
///   breadboard.module_library.Pulse,
///   breadboard.module_library.Bool,
///   breadboard.module_library.Int,
///   breadboard.module_library.Float,
///   breadboard.module_library.String,
///   breadboard.module_library.Entity,
///   breadboard.module_library.Vec3,
///   breadboard.module_library.Vec4,
///   breadboard.module_library.SoundHandle,
///   breadboard.module_library.AudioChannel,
///   [... list all input edge types here...]
///   // Note: The only edge definitions from the module library required
///   // by the default entity factory are the basic types (Pulse, Bool, Int,
///   // Float and String) as well as OutputEdgeTarget. The rest are optional
///   // and can be replaced with your own versions if you wish.
/// }
/// table InputEdgeDef {
///   edge:InputType;
/// }
/// table NodeDef {
///   module:string;
///   name:string;
///   input_edge_list:[InputEdgeDef];
/// }
/// table graphDef {
///   node_list:[NodeDef];
/// }
/// root_type GraphDef;
///
/// IMPORTANT: In the CPP file that includes this, you must remember to #define
/// BREADBOARD_FACTORY_TYPE_NAMESPACE to the namespace your generated
/// FlatBuffers data types are in before including this file, and to include the
/// generated header file before including this file.
///
/// For example, your project's default_graph_factory.cpp file should look
/// something like this:
///
/// /* default_module_factory.cpp */
/// #include "mygame_graph_generated.h"
///
/// #define BREADBOARD_FACTORY_TYPE_NAMESPACE mygame
/// #include "module_library/default_module_factory.inc"

#ifndef BREADBOARD_FACTORY_TYPE_NAMESPACE
#error You must #define BREADBOARD_FACTORY_TYPE_NAMESPACE to the namespace \
       your generated FlatBuffers types are in before including this file.
#endif

#include "module_library/default_graph_factory.h"

#include <string>

#include "breadboard/graph.h"
#include "breadboard/module_registry.h"
#include "fplbase/utilities.h"
#include "graph_generated.h"
#include "mathfu/glsl_mappings.h"
#include "mathfu/vector.h"
#include "pindrop/pindrop.h"

namespace breadboard {
namespace module_library {

static bool ParseType(
    DefaultGraphFactory* graph_factory, unsigned int node_index,
    unsigned int edge_index, breadboard::Graph* graph,
    const BREADBOARD_FACTORY_TYPE_NAMESPACE::InputEdgeDef* edge_def) {
  (void)graph_factory;
  switch (edge_def->edge_type()) {
    case BREADBOARD_FACTORY_TYPE_NAMESPACE::
        InputType_breadboard_module_library_OutputEdgeTarget: {
      // OutputEdgeTargets are a special case - they point at the output of
      // another node and so they do not need to store any default value.
      break;
    }
    case BREADBOARD_FACTORY_TYPE_NAMESPACE::
        InputType_breadboard_module_library_Pulse: {
      // Pulses have no data associated with them.
      break;
    }
    case BREADBOARD_FACTORY_TYPE_NAMESPACE::
        InputType_breadboard_module_library_Bool: {
      const breadboard::module_library::Bool* default_bool =
          static_cast<const breadboard::module_library::Bool*>(
              edge_def->edge());
      graph->SetDefaultValue<bool>(node_index, edge_index,
                                   default_bool->value() != 0);
      break;
    }
    case BREADBOARD_FACTORY_TYPE_NAMESPACE::
        InputType_breadboard_module_library_Int: {
      const breadboard::module_library::Int* default_int =
          static_cast<const breadboard::module_library::Int*>(edge_def->edge());
      graph->SetDefaultValue<int>(node_index, edge_index, default_int->value());
      break;
    }
    case BREADBOARD_FACTORY_TYPE_NAMESPACE::
        InputType_breadboard_module_library_Float: {
      const breadboard::module_library::Float* default_float =
          static_cast<const breadboard::module_library::Float*>(
              edge_def->edge());
      graph->SetDefaultValue<float>(node_index, edge_index,
                                    default_float->value());
      break;
    }
    case BREADBOARD_FACTORY_TYPE_NAMESPACE::
        InputType_breadboard_module_library_String: {
      const breadboard::module_library::String* default_string =
          static_cast<const breadboard::module_library::String*>(
              edge_def->edge());
      graph->SetDefaultValue<std::string>(node_index, edge_index,
                                          default_string->value()->c_str());
      break;
    }
#ifdef BREADBOARD_MODULE_LIBRARY_BUILD_CORGI_COMPONENT_LIBRARY
    case BREADBOARD_FACTORY_TYPE_NAMESPACE::
        InputType_breadboard_module_library_Entity: {
      // Entities can not be specified directly. Generally you would attach
      // to an output edge that supplied an entity, but it is legal to
      // leave an entity unspecified.
      break;
    }
#endif  // BREADBOARD_MODULE_LIBRARY_BUILD_CORGI
#ifdef BREADBOARD_MODULE_LIBRARY_BUILD_MATHFU
    case BREADBOARD_FACTORY_TYPE_NAMESPACE::
        InputType_breadboard_module_library_Vec3: {
      const breadboard::module_library::Vec3* default_vec3 =
          static_cast<const breadboard::module_library::Vec3*>(
              edge_def->edge());
      mathfu::vec3 vec(default_vec3->value()->x(), default_vec3->value()->y(),
                       default_vec3->value()->z());
      graph->SetDefaultValue<mathfu::vec3>(node_index, edge_index, vec);
      break;
    }
    case BREADBOARD_FACTORY_TYPE_NAMESPACE::
        InputType_breadboard_module_library_Vec4: {
      const breadboard::module_library::Vec4* default_vec4 =
          static_cast<const breadboard::module_library::Vec4*>(
              edge_def->edge());
      mathfu::vec4 vec(default_vec4->value()->x(), default_vec4->value()->y(),
                       default_vec4->value()->z(), default_vec4->value()->w());
      graph->SetDefaultValue<mathfu::vec4>(node_index, edge_index, vec);
      break;
    }
#endif  // BREADBOARD_MODULE_LIBRARY_BUILD_MATHFU
#ifdef BREADBOARD_MODULE_LIBRARY_BUILD_PINDROP
    case BREADBOARD_FACTORY_TYPE_NAMESPACE::
        InputType_breadboard_module_library_SoundHandle: {
      pindrop::AudioEngine* audio_engine = graph_factory->audio_engine();
      const breadboard::module_library::SoundHandle* default_sound_handle =
          static_cast<const breadboard::module_library::SoundHandle*>(
              edge_def->edge());
      pindrop::SoundHandle sound_handle =
          audio_engine->GetSoundHandle(default_sound_handle->value()->c_str());
      graph->SetDefaultValue<pindrop::SoundHandle>(node_index, edge_index,
                                                   sound_handle);
      break;
    }
    case BREADBOARD_FACTORY_TYPE_NAMESPACE::
        InputType_breadboard_module_library_AudioChannel: {
      // AudioChannels can not be specified directly. Generally you would attach
      // to an output edge that supplied an audio channel, but it is legal to
      // leave an audio channel unspecified.
      break;
    }
#endif  // BREADBOARD_MODULE_LIBRARY_BUILD_PINDROP
    default: {
      // Not all types can necessarily be loaded from data. Error out if
      // attempting to load these types.
      fplbase::LogError(
          "Cannot load data of unknown type from data files. (Got %i)",
          edge_def->edge_type());
      return false;
    }
  }
  return true;
}

bool DefaultGraphFactory::ParseData(breadboard::ModuleRegistry* module_registry,
                                    breadboard::Graph* graph,
                                    const std::string* data) {
  const BREADBOARD_FACTORY_TYPE_NAMESPACE::GraphDef* graph_def =
      BREADBOARD_FACTORY_TYPE_NAMESPACE::GetGraphDef(data->c_str());
  for (size_t i = 0; i != graph_def->node_list()->size(); ++i) {
    // Get Module.
    const BREADBOARD_FACTORY_TYPE_NAMESPACE::NodeDef* node_def =
        graph_def->node_list()->Get(static_cast<flatbuffers::uoffset_t>(i));
    const char* module_name = node_def->module()->c_str();
    const breadboard::Module* module = module_registry->GetModule(module_name);
    if (module == nullptr) return false;

    // Get NodeSignature.
    const char* node_sig_name = node_def->name()->c_str();
    const breadboard::NodeSignature* node_sig =
        module->GetNodeSignature(node_sig_name);
    if (node_sig == nullptr) return false;

    // Create Node and fill in edges.
    breadboard::Node* node = graph->AddNode(node_sig);
    auto edges = node_def->input_edge_list();
    if (edges != nullptr) {
      for (auto iter = edges->begin(); iter != edges->end(); ++iter) {
        const BREADBOARD_FACTORY_TYPE_NAMESPACE::InputEdgeDef* edge_def = *iter;
        node->input_edges().push_back(breadboard::InputEdge());
        if (edge_def->edge_type() ==
            BREADBOARD_FACTORY_TYPE_NAMESPACE::
                InputType_breadboard_module_library_OutputEdgeTarget) {
          const breadboard::module_library::OutputEdgeTarget* target =
              static_cast<const breadboard::module_library::OutputEdgeTarget*>(
                  edge_def->edge());
          node->input_edges().back().SetTarget(target->node_index(),
                                               target->edge_index());
        }
      }
    }
  }

  // Finalize Nodes and fill in default values.
  if (!graph->FinalizeNodes()) return false;

  // Set up default values.
  for (unsigned int node_index = 0;
       node_index != static_cast<unsigned int>(graph_def->node_list()->size());
       ++node_index) {
    const BREADBOARD_FACTORY_TYPE_NAMESPACE::NodeDef* node_def =
        graph_def->node_list()->Get(node_index);
    auto edges = node_def->input_edge_list();
    if (edges != nullptr) {
      for (unsigned int edge_index = 0;
           edge_index < static_cast<unsigned int>(edges->size());
           ++edge_index) {
        const BREADBOARD_FACTORY_TYPE_NAMESPACE::InputEdgeDef* edge_def =
            edges->Get(edge_index);
        if (!ParseType(this, node_index, edge_index, graph, edge_def)) {
          return false;
        }
      }
    }
  }
  return true;
}

}  // module_library
}  // breadboard
