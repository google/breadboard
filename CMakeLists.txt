# Copyright 2015 Google Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
cmake_minimum_required(VERSION 2.8.12)

project(breadboard)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Temporary files (like object files) created while compiling projects.
set(tmp_dir ${CMAKE_CURRENT_BINARY_DIR}/obj)

# Build the common module library.
set(breadboard_standalone_mode OFF)
if("${CMAKE_CURRENT_LIST_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  set(breadboard_standalone_mode ON)
  message(STATUS "breadboard standalone: building library and samples")
endif()
option(breadboard_build_samples "Build the breadboard sample executables."
       ${breadboard_standalone_mode})

option(breadboard_build_module_library
    "Build a library of standard modules along with Breadboard" ON)

# Include component library modules
option(breadboard_build_corgi_component_library_module
    "Build a library of standard components along with the entity system" ON)

# Include a pindrop audio module
option(breadboard_build_pindrop_module
    "Build a library of standard components along with the entity system" ON)

# Include a mathfu vector module
option(breadboard_build_mathfu_module
    "Build a library of standard components along with the entity system" ON)

# Call fplutil to get locations of dependencies and set common build settings.
include("cmake/find_fplutil.cmake")
include("${fplutil_dir}/buildutil/cmake_common.txt")
set_common_build_variables()

# Breadboard files.
set(breadboard_SRCS
    include/breadboard/base_node.h
    include/breadboard/event.h
    include/breadboard/graph.h
    include/breadboard/graph_factory.h
    include/breadboard/graph_state.h
    include/breadboard/log.h
    include/breadboard/memory_buffer.h
    include/breadboard/module.h
    include/breadboard/module_registry.h
    include/breadboard/node.h
    include/breadboard/node_arguments.h
    include/breadboard/node_signature.h
    include/breadboard/type.h
    include/breadboard/type_registry.h
    include/breadboard/version.h
    src/breadboard/event.cpp
    src/breadboard/graph.cpp
    src/breadboard/graph_factory.cpp
    src/breadboard/graph_state.cpp
    src/breadboard/log.cpp
    src/breadboard/module.cpp
    src/breadboard/module_registry.cpp
    src/breadboard/node.cpp
    src/breadboard/node_arguments.cpp
    src/breadboard/node_signature.cpp
    src/breadboard/type_registry.cpp
    src/breadboard/version.cpp)

set(breadboard_common_modules_SRCS
    include/breadboard/modules/common.h
    include/breadboard/modules/debug.h
    include/breadboard/modules/logic.h
    include/breadboard/modules/math.h
    include/breadboard/modules/string.h
    src/modules/common.cpp
    src/modules/debug.cpp
    src/modules/logic.cpp
    src/modules/math.cpp
    src/modules/string.cpp)

# Includes for this project.
include_directories(include)
include_directories(${dependencies_fplutil_dir}/libfplutil/include)

add_library(breadboard ${breadboard_SRCS} ${breadboard_common_modules_SRCS})
target_link_libraries(breadboard)

if (breadboard_build_module_library)
  # Include FlatBuffers in this project.
  if(NOT TARGET flatc)
    set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "")
    add_subdirectory("${dependencies_flatbuffers_dir}" ${tmp_dir}/flatbuffers)
  endif()

  # Generate rules to build the set of output files from the set of input
  # schema files.
  set(MODULE_LIBRARY_FLATBUFFERS_GENERATED_INCLUDES_DIR
      ${CMAKE_CURRENT_BINARY_DIR}/include/module_library)
  file(GLOB_RECURSE
    MODULE_LIBRARY_FLATBUFFERS_SCHEMAS
    module_library/schemas/*.fbs)
  unset(FLATBUFFERS_GENERATED_INCLUDES)

  if (NOT TARGET flatc)
    add_subdirectory("${dependencies_flatbuffers_dir}" ${tmp_dir}/flatbuffers)
  endif()

  set(module_library_flatbuffer_dependencies
      "${dependencies_fplbase_dir}/schemas")
  build_flatbuffers("${MODULE_LIBRARY_FLATBUFFERS_SCHEMAS}"
                    "${module_library_flatbuffer_dependencies}"
                    breadboard_module_library_generated_includes
                    ""
                    ${MODULE_LIBRARY_FLATBUFFERS_GENERATED_INCLUDES_DIR}
                    ""
                    "")

  set(breadboard_module_library_DEPS breadboard)
  set(breadboard_module_library_SRCS
      ${breadboard_module_library_SRCS}
      module_library/include/module_library/default_graph_factory.h)

  # Include vector module.
  if (breadboard_build_mathfu_module)
    set(mathfu_build_benchmarks OFF CACHE BOOL "")
    set(mathfu_build_tests OFF CACHE BOOL "")
    add_subdirectory("${dependencies_mathfu_dir}" ${tmp_dir}/mathfu)
    include_directories(${dependencies_mathfu_dir}/include)
    set(breadboard_module_library_SRCS
        ${breadboard_module_library_SRCS}
        module_library/include/module_library/vec.h
        module_library/src/vec.cpp)
  endif()

  # Include modules for the components in the component library.
  if (breadboard_build_corgi_component_library_module)
    if (NOT TARGET fplbase)
      add_subdirectory("${dependencies_fplbase_dir}" ${tmp_dir}/fplbase)
    endif()
    if (NOT TARGET corgi)
      add_subdirectory(${dependencies_corgi_dir} ${tmp_dir}/corgi)
    endif()
    get_property(CORGI_COMPONENT_LIBRARY_FLATBUFFERS_GENERATED_INCLUDES_DIR
                 TARGET corgi_component_library_generated_includes
                 PROPERTY GENERATED_INCLUDES_DIR)
    include_directories(
      ${CORGI_COMPONENT_LIBRARY_FLATBUFFERS_GENERATED_INCLUDES_DIR})
    get_property(FPLBASE_FLATBUFFERS_GENERATED_INCLUDES_DIR
                 TARGET fplbase_generated_includes
                 PROPERTY GENERATED_INCLUDES_DIR)
    include_directories(${FPLBASE_FLATBUFFERS_GENERATED_INCLUDES_DIR})
    include_directories(${dependencies_bulletphysics_distr_dir}/src)
    include_directories(${dependencies_corgi_component_library_dir}/include)
    include_directories(${dependencies_corgi_dir}/include)
    include_directories(${dependencies_flatbuffers_dir}/include)
    include_directories(${dependencies_fplbase_dir}/include)
    include_directories(${dependencies_motive_dir}/include)
    include_directories(module_library/include)

    set(breadboard_module_library_DEPS
        ${breadboard_module_library_DEPS}
        corgi_component_library)
    set(breadboard_module_library_SRCS
        ${breadboard_module_library_SRCS}
        module_library/include/module_library/animation.h
        module_library/include/module_library/entity.h
        module_library/include/module_library/physics.h
        module_library/include/module_library/rendermesh.h
        module_library/include/module_library/transform.h
        module_library/src/animation.cpp
        module_library/src/entity.cpp
        module_library/src/physics.cpp
        module_library/src/rendermesh.cpp
        module_library/src/transform.cpp)
  endif()

  # Include pindrop audio module.
  if (breadboard_build_pindrop_module)
    include_directories(${dependencies_pindrop_dir}/include)
    set(breadboard_module_library_SRCS
        ${breadboard_module_library_SRCS}
        module_library/include/module_library/audio.h
        module_library/src/audio.cpp)
  endif()

  include_directories(module_library/include)
  add_library(breadboard_module_library ${breadboard_module_library_SRCS})
  add_dependencies(breadboard_module_library ${breadboard_module_library_DEPS})
  if (breadboard_build_mathfu_module)
    mathfu_configure_flags(breadboard_module_library)
  endif()
  target_link_libraries(breadboard_module_library ${breadboard_module_library_DEPS})

  # Macro defined here so that it can be used by all projects included
  function(breadboard_module_library_configure_flags project)
    if (breadboard_build_mathfu_module)
      set_property(TARGET ${project}
          APPEND PROPERTY COMPILE_DEFINITIONS
          BREADBOARD_MODULE_LIBRARY_BUILD_MATHFU)
    endif()
    if (breadboard_build_corgi_component_library_module)
      set_property(TARGET ${project}
          APPEND PROPERTY COMPILE_DEFINITIONS
          BREADBOARD_MODULE_LIBRARY_BUILD_CORGI_COMPONENT_LIBRARY)
    endif()
    if (breadboard_build_pindrop_module)
      set_property(TARGET ${project}
          APPEND PROPERTY COMPILE_DEFINITIONS
          BREADBOARD_MODULE_LIBRARY_BUILD_PINDROP)
    endif()
  endfunction(breadboard_module_library_configure_flags)
endif()

if(breadboard_build_samples)
  add_subdirectory(samples)
endif()
