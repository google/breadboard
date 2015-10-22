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

LOCAL_MODULE := module_library
LOCAL_ARM_MODE := arm
LOCAL_STATIC_LIBRARIES := libcomponent_library libpindrop libbreadboard
LOCAL_SHARED_LIBRARIES :=

MODULES_RELATIVE_DIR := ..
MODULES_DIR := $(LOCAL_PATH)/$(MODULES_RELATIVE_DIR)

include $(MODULES_DIR)/jni/android_config.mk

LOCAL_EXPORT_C_INCLUDES := \
  $(MODULES_DIR)/include \
  $(DEPENDENCIES_FPLUTIL_DIR)/libfplutil/include \

LOCAL_C_INCLUDES := \
  $(LOCAL_EXPORT_C_INCLUDES) \
  $(DEPENDENCIES_BREADBOARD_DIR)/include \
  $(DEPENDENCIES_BULLETPHYSICS_DIR)/src \
  $(DEPENDENCIES_ENTITY_DIR)/include \
  $(DEPENDENCIES_FLATBUFFERS_DIR)/include \
  $(DEPENDENCIES_MATHFU_DIR)/include \
  $(DEPENDENCIES_MOTIVE_DIR)/include \
  $(MODULES_DIR)/src

LOCAL_SRC_FILES := \
  $(MODULES_RELATIVE_DIR)/src/module_library/animation.cpp \
  $(MODULES_RELATIVE_DIR)/src/module_library/audio.cpp \
  $(MODULES_RELATIVE_DIR)/src/module_library/common.cpp \
  $(MODULES_RELATIVE_DIR)/src/module_library/debug.cpp \
  $(MODULES_RELATIVE_DIR)/src/module_library/entity.cpp \
  $(MODULES_RELATIVE_DIR)/src/module_library/logic.cpp \
  $(MODULES_RELATIVE_DIR)/src/module_library/math.cpp \
  $(MODULES_RELATIVE_DIR)/src/module_library/physics.cpp \
  $(MODULES_RELATIVE_DIR)/src/module_library/string.cpp \
  $(MODULES_RELATIVE_DIR)/src/module_library/transform.cpp \
  $(MODULES_RELATIVE_DIR)/src/module_library/vec3.cpp

include $(BUILD_STATIC_LIBRARY)

$(call import-add-path,$(DEPENDENCIES_FLATBUFFERS_DIR)/..)
$(call import-add-path,$(DEPENDENCIES_MATHFU_DIR)/..)
$(call import-add-path,$(DEPENDENCIES_PINDROP_DIR)/..)

$(call import-module,flatbuffers/android/jni)
$(call import-module,mathfu/jni)
$(call import-module,breadboard/jni)
$(call import-module,android/cpufeatures)
