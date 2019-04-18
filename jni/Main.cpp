/*  Let's go full native!  */

#define APP_TITLE "lithium"

// Stuff for recieve log message
#include <android/log.h>
#define LOG_INFO(...)  __android_log_print(ANDROID_LOG_INFO,  APP_TITLE, __VA_ARGS__)
#define LOG_WARN(...)  __android_log_print(ANDROID_LOG_WARN,  APP_TITLE, __VA_ARGS__)
#define LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, APP_TITLE, __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, APP_TITLE, __VA_ARGS__)

// TODO:
// - Replace iterators to "it" in code for small
// - refacrore contools event (!)
// - Grioup widgets into one object
// - Font widget
// - Dragable widget + status for easy debug!
// - check on big on null texteres
// - ...1010

// Stuff for status state
typedef unsigned long int status;
const status STATUS_OK         =  0;
const status STATUS_ERROR      = -1;
const status STATUS_EXIT       = -2;
const status STATUS_NOT_LOADED = -3;

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
void android_main(android_app* pApplication) {
	LOG_INFO("--- Let's go full native! ---");
	EventLoop lEventLoop(pApplication);
    Sensor lAccelerometer(lEventLoop, ASENSOR_TYPE_ACCELEROMETER);
    // Creates services.
    TimeManager lTimeManager;
    GraphicsManager lGraphicsManager(pApplication);
    InputManager lInputManager(pApplication, &lAccelerometer, &lGraphicsManager);
    SoundManager lSoundManager(pApplication);
    // Fills the context.
    Context lContext = { &lGraphicsManager, &lInputManager, &lSoundManager, &lTimeManager };
    // Toggle fullscreen
    ANativeActivity_setWindowFlags(pApplication->activity, AWINDOW_FLAG_FULLSCREEN, AWINDOW_FLAG_FULLSCREEN);
    // Starts the game loop.
    Engine lEngine(&lContext);
    lEventLoop.run(&lEngine, &lInputManager);
	LOG_INFO("--- Bye! ---");
}
