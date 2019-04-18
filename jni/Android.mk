LOCAL_PATH := $(call my-dir)
$(info $(LOCAL_PATH))

include $(CLEAR_VARS)
LOCAL_MODULE := lithium
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_CPPFLAGS := -Werror
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lOpenSLES 
LOCAL_STATIC_LIBRARIES := android_native_app_glue libpng
include $(BUILD_SHARED_LIBRARY)

$(call import-add-path, $(LOCAL_PATH))
$(call import-module,libpng)
$(call import-module,android/native_app_glue)