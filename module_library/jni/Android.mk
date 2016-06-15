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

LOCAL_PATH := $(call my-dir)/..

BREADBOARD_MODULE_LIBRARY_DIR := $(LOCAL_PATH)

include $(BREADBOARD_MODULE_LIBRARY_DIR)/jni/android_config.mk
include $(DEPENDENCIES_FLATBUFFERS_DIR)/android/jni/include.mk

# realpath-portable From flatbuffers/android/jni/include.mk
LOCAL_PATH := $(call realpath-portable,$(LOCAL_PATH))
BREADBOARD_MODULE_LIBRARY_DIR := $(LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE := breadboard_module_library
LOCAL_ARM_MODE := arm
LOCAL_STATIC_LIBRARIES := \
  libcorgi_component_library \
  libpindrop \
  libbreadboard \
  libmathfu
LOCAL_SHARED_LIBRARIES :=

LOCAL_CPPFLAGS := \
  -DBREADBOARD_MODULE_LIBRARY_BUILD_MATHFU \
  -DBREADBOARD_MODULE_LIBRARY_BUILD_CORGI_COMPONENT_LIBRARY \
  -DBREADBOARD_MODULE_LIBRARY_BUILD_PINDROP

LOCAL_EXPORT_CPPFLAGS := $(LOCAL_CPPFLAGS)

BREADBOARD_MODULE_LIBRARY_GENERATED_OUTPUT_DIR := \
    $(BREADBOARD_MODULE_LIBRARY_DIR)/gen/include

LOCAL_EXPORT_C_INCLUDES := \
  $(DEPENDENCIES_FPLUTIL_DIR)/libfplutil/include \
  $(BREADBOARD_MODULE_LIBRARY_DIR)/include \
  $(BREADBOARD_MODULE_LIBRARY_GENERATED_OUTPUT_DIR)

LOCAL_C_INCLUDES := \
  $(LOCAL_EXPORT_C_INCLUDES) \
  $(DEPENDENCIES_BULLETPHYSICS_DIR)/src \
  $(DEPENDENCIES_CORGI_DIR)/include \
  $(DEPENDENCIES_CORGI_DIR)/component_library/include \
  $(DEPENDENCIES_FLATBUFFERS_DIR)/include \
  $(DEPENDENCIES_MATHFU_DIR)/include \
  $(DEPENDENCIES_MOTIVE_DIR)/include \
  $(DEPENDENCIES_PINDROP_DIR)/include \
  $(DEPENDENCIES_FPLBASE_DIR)/include

LOCAL_SRC_FILES := \
  src/animation.cpp \
  src/audio.cpp \
  src/entity.cpp \
  src/physics.cpp \
  src/rendermesh.cpp \
  src/transform.cpp \
  src/vec.cpp

BREADBOARD_MODULE_LIBRARY_SCHEMA_DIR := \
    $(BREADBOARD_MODULE_LIBRARY_DIR)/schemas
BREADBOARD_MODULE_LIBRARY_SCHEMA_INCLUDE_DIRS := \
    $(DEPENDENCIES_FPLBASE_DIR)/schemas

BREADBOARD_MODULE_LIBRARY_SCHEMA_FILES := \
  $(BREADBOARD_MODULE_LIBRARY_SCHEMA_DIR)/common_modules.fbs \
  $(BREADBOARD_MODULE_LIBRARY_SCHEMA_DIR)/corgi_module.fbs \
  $(BREADBOARD_MODULE_LIBRARY_SCHEMA_DIR)/mathfu_module.fbs \
  $(BREADBOARD_MODULE_LIBRARY_SCHEMA_DIR)/pindrop_module.fbs

ifeq (,$(BREADBOARD_MODULE_LIBRARY_RUN_ONCE))
BREADBOARD_MODULE_LIBRARY_RUN_ONCE := 1
$(call flatbuffers_header_build_rules,\
  $(BREADBOARD_MODULE_LIBRARY_SCHEMA_FILES),\
  $(BREADBOARD_MODULE_LIBRARY_SCHEMA_DIR),\
  $(BREADBOARD_MODULE_LIBRARY_GENERATED_OUTPUT_DIR),\
  $(BREADBOARD_MODULE_LIBRARY_SCHEMA_INCLUDE_DIRS),\
  $(LOCAL_SRC_FILES),\
  breadboard_module_library_generated_includes,\
  corgi_component_library_generated_includes \
  fplbase_generated_includes)
endif

include $(BUILD_STATIC_LIBRARY)

$(call import-add-path,$(DEPENDENCIES_BREADBOARD_DIR)/..)
$(call import-add-path,$(DEPENDENCIES_FPLBASE_DIR)/..)
$(call import-add-path,$(DEPENDENCIES_FLATBUFFERS_DIR)/..)
$(call import-add-path,$(DEPENDENCIES_MATHFU_DIR)/..)
$(call import-add-path,$(DEPENDENCIES_PINDROP_DIR)/..)
$(call import-add-path,$(DEPENDENCIES_CORGI_DIR)/..)

$(call import-module,fplbase/jni)
$(call import-module,flatbuffers/android/jni)
$(call import-module,mathfu/jni)
$(call import-module,breadboard/jni)
$(call import-module,corgi/component_library/jni)
$(call import-module,android/cpufeatures)
