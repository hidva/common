LOCAL_PATH_BAK := $(LOCAL_PATH)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := pp_qq_common_log

LOCAL_SRC_FILES := pp_qq_log.cc

LOCAL_CPP_FEATURES := rtti exceptions

include $(BUILD_STATIC_LIBRARY)


LOCAL_PATH := $(LOCAL_PATH_BAK)
