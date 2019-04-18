LOCAL_PATH := $(call my-dir)
$(info Native root: $(LOCAL_PATH))
include $(CLEAR_VARS)

define walk
  $(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
endef

LOCAL_MODULE := lithium
ALL_FILES = $(call walk, $(LOCAL_PATH))
FILE_LIST := $(filter %.cpp, $(ALL_FILES))
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_CPPFLAGS := -Werror
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lOpenSLES -latomic
LOCAL_STATIC_LIBRARIES := android_native_app_glue libpng
include $(BUILD_SHARED_LIBRARY)

$(call import-add-path, $(LOCAL_PATH))
$(call import-module,libpng)
$(call import-module,android/native_app_glue)