# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
EXTERN_LIB_PATH = externlib

LOCAL_PATH := $(call my-dir)

##emvbase
include $(CLEAR_VARS)
LOCAL_MODULE := szzt_emvbasejni
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(EXTERN_LIB_PATH)/libszzt_emvbasejni.so
include $(PREBUILT_SHARED_LIBRARY)

##emvmath
include $(CLEAR_VARS)
LOCAL_MODULE := szzt_emvmathjni
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(EXTERN_LIB_PATH)/libszzt_emvmathjni.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := szzt_jcbjni
LOCAL_C_INCLUDES := $(LOCAL_PATH)/dlljcb/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/dlljcb/externinc/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/dlljcb/privateinc/


MY_CPP_LIST := $(wildcard $(LOCAL_PATH)/*/src/*.c)

LOCAL_SRC_FILES := $(MY_CPP_LIST:$(LOCAL_PATH)/%=%)

LOCAL_SHARED_LIBRARIES := szzt_emvbasejni
LOCAL_SHARED_LIBRARIES += szzt_emvmathjni

#LOCAL_CFLAGS := -Dmemcpy -Dmemset
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)




