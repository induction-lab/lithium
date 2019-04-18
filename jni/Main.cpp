/*  Let's go full native!  */

#define APP_TITLE "lithium"

// Stuff for recieve log message
#include <stdio.h>
#include <android/log.h>
#define LOG_INFO(x...) do { \
	char buf[512]; \
	sprintf(buf, x); \
	__android_log_print(ANDROID_LOG_INFO, APP_TITLE, "%s: %s", __FILE__, buf); \
} while (0)
#define LOG_WARN(x...) do { \
	char buf[512]; \
	sprintf(buf, x); \
	__android_log_print(ANDROID_LOG_WARN, APP_TITLE, "%s: %s", __FILE__, buf); \
} while (0)
#define LOG_ERROR(x...) do { \
	char buf[512]; \
	sprintf(buf, x); \
	__android_log_print(ANDROID_LOG_ERROR, APP_TITLE, "%s: %s", __FILE__, buf); \
} while (0)
#define LOG_DEBUG(x...) do { \
	char buf[512]; \
	sprintf(buf, x); \
	__android_log_print(ANDROID_LOG_DEBUG, APP_TITLE, "%s: %s", __FILE__, buf); \
} while (0)

// Object deletion macro
#define SAFE_DELETE(x) { delete x; x = NULL; }

// Array deletion macro
#define SAFE_DELETE_ARRAY(x) { delete[] x; x = NULL; }

// Stuff for status state
typedef unsigned long int status;
const status STATUS_OK         =  0;
const status STATUS_ERROR      = -1;
const status STATUS_EXIT       = -2;

// Base native app glue.
#include <android_native_app_glue.h>
static android_app* application;

// Application default config.
struct ConfigData {
    bool musicOn = false;
};
ConfigData* configData;

// Type of ui mode.
#include "android/configuration.h"
static int32_t uiModeType;

#include "Geometry.h"
#include "TimeManager.h"
#include "TweenManager.h"
#include "GraphicsManager.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "EventLoop.h"
#include "Activity.h"

// TODO:
// - besier ...
// - game object implemennt ...
// - render batch ? ...
// - disable sensors ...
// - stop sound queue due shange scene ...

// Android entry point.
void android_main(android_app* app) {
	LOG_INFO("--- Let's go full native! ---");
	LOG_INFO("Build date: %s %s", GetBuildDate(), __TIME__);
    // Toggle fullscreen.
    ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_FULLSCREEN, AWINDOW_FLAG_FULLSCREEN);
	// Application details provided by Android.
	application = app;
    // Get ui type.
    AConfiguration* configuration = AConfiguration_new();
    AConfiguration_fromAssetManager(configuration, application->activity->assetManager);
    uiModeType = AConfiguration_getUiModeType(configuration);
    AConfiguration_delete(configuration);
    // Read config data.
    readConfig();
    // Starts the game loop.
	EventLoop* eventLoop = new EventLoop();
	eventLoop->run(new Activity);
	SAFE_DELETE(eventLoop);
    // Save config data.
    writeConfig();
	LOG_INFO("--- Bye! ---");
}
