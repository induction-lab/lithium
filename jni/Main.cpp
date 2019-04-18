/*  Let's go full native!  */

#define APP_TITLE "lithium"

// Stuff for recieve log message
#include <stdio.h>
#include <android/log.h>
#define LOG_INFO(...)  __android_log_print(ANDROID_LOG_INFO,  APP_TITLE, __VA_ARGS__)
#define LOG_WARN(...)  __android_log_print(ANDROID_LOG_WARN,  APP_TITLE, __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, APP_TITLE, __VA_ARGS__)
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

#include <android_native_app_glue.h>
android_app* application;

#include "Geometry.h"
#include "Singleton.h"
#include "TimeManager.h"
#include "GraphicsManager.h"
#include "InputManager.h"
#include "Platform.h"
#include "Activity.h"

// Android entry point
void android_main(android_app* app) {
	LOG_INFO("--- Let's go full native! ---");
	LOG_INFO("Build date: %s %s", GetBuildDate(), __TIME__);	
    // Toggle fullscreen.
    // ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_FULLSCREEN, AWINDOW_FLAG_FULLSCREEN);
	// Application details provided by Android.
	application = app;
    // Starts the game loop.
	Platform* platform = new Platform();
	platform->run(new Activity);
	SAFE_DELETE(platform);
	LOG_INFO("--- Bye! ---");
}
