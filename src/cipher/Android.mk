
LOCAL_PATH_BAK := $(LOCAL_PATH)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := pp_qq_common_cipher

LOCAL_SRC_FILES := aes_cipher.cc block_cipher.cc counter.cc ctr_cipher_mode.cc \
	icipher.cc memxor.cc

LOCAL_CPP_FEATURES := rtti exceptions

include $(BUILD_STATIC_LIBRARY)


LOCAL_PATH := $(LOCAL_PATH_BAK)
