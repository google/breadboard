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

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := breadboard
LOCAL_ARM_MODE := arm
LOCAL_STATIC_LIBRARIES :=
LOCAL_SHARED_LIBRARIES :=

BREADBOARD_RELATIVE_DIR := ..
BREADBOARD_DIR := $(LOCAL_PATH)/$(BREADBOARD_RELATIVE_DIR)

include $(BREADBOARD_DIR)/jni/android_config.mk

LOCAL_EXPORT_C_INCLUDES := $(BREADBOARD_DIR)/include

LOCAL_C_INCLUDES := \
  $(DEPENDENCIES_FPLUTIL_DIR)/libfplutil/include \
  $(BREADBOARD_DIR)/src \
  $(LOCAL_EXPORT_C_INCLUDES)

LOCAL_SRC_FILES := \
  $(BREADBOARD_RELATIVE_DIR)/src/breadboard/event.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/breadboard/graph.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/breadboard/graph_factory.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/breadboard/graph_state.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/breadboard/log.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/breadboard/module.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/breadboard/module_registry.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/breadboard/node.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/breadboard/node_arguments.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/breadboard/node_signature.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/breadboard/type_registry.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/modules/common.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/modules/debug.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/modules/logic.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/modules/math.cpp \
  $(BREADBOARD_RELATIVE_DIR)/src/modules/string.cpp

include $(BUILD_STATIC_LIBRARY)
