LOCAL_PATH_BAK := $(LOCAL_PATH)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := pp_qq_common_log

ifdef $(LOCAL_MODULE)_c_includes
LOCAL_C_INCLUDES := $($(LOCAL_MODULE)_c_includes)
endif

ifdef $(LOCAL_MODULE)_cflags
LOCAL_CFLAGS := $($(LOCAL_MODULE)_cflags)
endif

ifdef $(LOCAL_MODULE)_cppflags
LOCAL_CPPFLAGS := $($(LOCAL_MODULE)_cppflags)
endif

ifdef $(LOCAL_MODULE)_export_cflags
LOCAL_EXPORT_CFLAGS := $($(LOCAL_MODULE)_export_cflags)
endif

ifdef $(LOCAL_MODULE)_export_cppflags
LOCAL_EXPORT_CPPFLAGS := $($(LOCAL_MODULE)_export_cppflags)
endif

ifdef $(LOCAL_MODULE)_export_c_includes
LOCAL_EXPORT_C_INCLUDES := $($(LOCAL_MODULE)_export_c_includes)
endif

LOCAL_STATIC_LIBRARIES := pp_qq_common_formatter

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../
LOCAL_SRC_FILES := pp_qq_log.cc

LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/../

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

module_for_test := pp_qq_common_log

LOCAL_MODULE := $(module_for_test)_test
LOCAL_SRC_FILES := pp_qq_log_test.cc
LOCAL_STATIC_LIBRARIES := googletest_static $(module_for_test)

include $(BUILD_STATIC_LIBRARY)

LOCAL_PATH := $(LOCAL_PATH_BAK)
