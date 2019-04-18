LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := lithium
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_CPPFLAGS := -Werror
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lOpenSLES 
LOCAL_STATIC_LIBRARIES := android_native_app_glue libpng
include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,libpng)