/*  Let's go full native!  */

#define APP_TITLE "lithium"

// Stuff for recieve log message
#include <android/log.h>
#define LOG_INFO(...)  __android_log_print(ANDROID_LOG_INFO,  APP_TITLE, __VA_ARGS__)
#define LOG_WARN(...)  __android_log_print(ANDROID_LOG_WARN,  APP_TITLE, __VA_ARGS__)
#define LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, APP_TITLE, __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, APP_TITLE, __VA_ARGS__)

// Stuff for status state
typedef unsigned long int status;
const status STATUS_OK         =  0;
const status STATUS_ERROR      = -1;
const status STATUS_EXIT       = -2;

// Basic context services
class GraphicsManager;
class InputManager;
class SoundManager;
class TimeManager;

struct Context {
    GraphicsManager* mGraphicsManager;
    InputManager*    mInputManager;
    SoundManager*    mSoundManager; 
    TimeManager*     mTimeManager;
};

#include "EventLoop.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "GraphicsManager.h"
#include "Engine.h"

// Android entry point
void android_main(android_app* app) {
	LOG_INFO("--- Let's go full native! ---");
	LOG_INFO("Build date: %s %s", GetBuildDate(), __TIME__);	
	EventLoop eventLoop(app);
    // Creates services.
    TimeManager timeManager;
    GraphicsManager graphicsManager(app);
    InputManager inputManager(app);
    SoundManager soundManager(app);
    // Fills the context.
    Context context = { &graphicsManager, &inputManager, &soundManager, &timeManager };
    // Toggle fullscreen
    ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_FULLSCREEN, AWINDOW_FLAG_FULLSCREEN);
    // Starts the game loop.
    Engine engine(&context);
    eventLoop.run(&engine, &inputManager);
	LOG_INFO("--- Bye! ---");
}
