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

BREADBOARD_DIR := $(LOCAL_PATH)/../..

# Pull in depedency config for fplutil.
include $(BREADBOARD_DIR)/jni/android_config.mk

include $(CLEAR_VARS)
LOCAL_MODULE := breadboard_event_counter
LOCAL_SRC_FILES := event_counter.cpp
LOCAL_STATIC_LIBRARIES := breadboard
LOCAL_WHOLE_STATIC_LIBRARIES:=android_native_app_glue libfplutil_main \
  libfplutil_print
LOCAL_EXPORT_C_INCLUDES:=$(DEPENDENCIES_FPLUTIL_DIR)/libfplutil/include
LOCAL_LDLIBS:=-llog -landroid
LOCAL_ARM_MODE := arm
include $(BUILD_SHARED_LIBRARY)

$(call import-add-path,$(DEPENDENCIES_FPLUTIL_DIR)/..)
$(call import-add-path,$(DEPENDENCIES_BREADBOARD_DIR)/..)
$(call import-module,android/native_app_glue)
$(call import-module,breadboard/jni)
$(call import-module,fplutil/libfplutil/jni)
