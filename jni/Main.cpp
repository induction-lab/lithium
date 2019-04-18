/*  Let's go full native!  */

#define APP_TITLE "lithium"

// Stuff for recieve log message
#include <android/log.h>
#define LOG_INFO(...)  __android_log_print(ANDROID_LOG_INFO,  APP_TITLE, __VA_ARGS__)
#define LOG_WARN(...)  __android_log_print(ANDROID_LOG_WARN,  APP_TITLE, __VA_ARGS__)
#define LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, APP_TITLE, __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, APP_TITLE, __VA_ARGS__)

// TODO:
// - All the new

#include "Math.h"
#include "Engine.h"

// Android entry point
void android_main(struct android_app* state) {
	LOG_INFO("--- Let's go full native! ---");
	app_dummy();
	Engine app(state);
	if (app.Initialize()) app.Run();
	LOG_INFO("--- Bye! ---");
}
