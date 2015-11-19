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

# TODO: Remove when the LOCAL_PATH expansion bug in the NDK is fixed.
# Portable version of $(realpath) that omits drive letters on Windows.
realpath-portable = $(join $(filter %:,$(subst :,: ,$1)),\
                      $(realpath $(filter-out %:,$(subst :,: ,$1))))

LOCAL_PATH := $(call realpath-portable,$(call my-dir)/..)

BREADBOARD_DIR := $(LOCAL_PATH)
include $(BREADBOARD_DIR)/jni/android_config.mk

include $(CLEAR_VARS)
LOCAL_MODULE := breadboard
LOCAL_ARM_MODE := arm

LOCAL_EXPORT_C_INCLUDES := $(BREADBOARD_DIR)/include

LOCAL_C_INCLUDES := \
  $(DEPENDENCIES_FPLUTIL_DIR)/libfplutil/include \
  $(BREADBOARD_DIR)/src \
  $(LOCAL_EXPORT_C_INCLUDES)

LOCAL_SRC_FILES := \
  src/breadboard/event.cpp \
  src/breadboard/graph.cpp \
  src/breadboard/graph_factory.cpp \
  src/breadboard/graph_state.cpp \
  src/breadboard/log.cpp \
  src/breadboard/module.cpp \
  src/breadboard/module_registry.cpp \
  src/breadboard/node.cpp \
  src/breadboard/node_arguments.cpp \
  src/breadboard/node_signature.cpp \
  src/breadboard/type_registry.cpp \
  src/breadboard/version.cpp \
  src/modules/common.cpp \
  src/modules/debug.cpp \
  src/modules/logic.cpp \
  src/modules/math.cpp \
  src/modules/string.cpp

include $(BUILD_STATIC_LIBRARY)
