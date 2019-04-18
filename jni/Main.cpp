/* Let's go full native! */

#define APP_TITLE "lithium"
#define DEBUG_MODE
#define SLOW_DOWN 1

// Stuff for recieve log message.
#include <stdio.h>
#include <android/log.h>
// Log.
#define LOG_INFO(x...) do { \
    char buf[512]; \
    sprintf(buf, x); \
    __android_log_print(ANDROID_LOG_INFO, APP_TITLE, "%s:%s(): %s", __FILE__, __FUNCTION__, buf); \
} while (0)
// Warn.
#define LOG_WARN(x...) do { \
    char buf[512]; \
    sprintf(buf, x); \
    __android_log_print(ANDROID_LOG_WARN, APP_TITLE, "%s:%s(): %s", __FILE__, __FUNCTION__, buf); \
} while (0)
// Error.
#define LOG_ERROR(x...) do { \
    char buf[512]; \
    sprintf(buf, x); \
    __android_log_print(ANDROID_LOG_ERROR, APP_TITLE, "%s:%s(): %s", __FILE__, __FUNCTION__, buf); \
} while (0)
// Debug.
#ifdef DEBUG_MODE
    #define LOG_DEBUG(x...) do { \
        char buf[512]; \
        sprintf(buf, x); \
        __android_log_print(ANDROID_LOG_DEBUG, APP_TITLE, "%s:%s(): %s", __FILE__, __FUNCTION__, buf); \
    } while (0)
#else
    #define LOG_DEBUG(x...) {}
#endif

// Object deletion macro.
#define SAFE_DELETE(x) { delete x; x = NULL; }

// Array deletion macro.
#define SAFE_DELETE_ARRAY(x) { delete[] x; x = NULL; }

// Stuff for status state.
typedef unsigned long int status;
const status STATUS_OK     =  0;
const status STATUS_ERROR  = -1;
const status STATUS_EXIT   = -2;

// Base native app glue.
#include <android_native_app_glue.h>
static android_app* application;

// Base 'match three' game params.
#define GRID_SIZE 6                 // optimal for watch screen
#define FRUITS_COUNT 7              // number base type fruits
#define MIN_MATCH_COUNT 3           // minimal match fruits
// Bonus match count fruits.
#define MIN_MATCH_FINE_COUNT 5
#define MIN_MATCH_WONDERFUL_COUNT 12
#define MIN_MATCH_EXELENT_COUNT 16
#define MIN_MATCH_UNBELIEVABLE_COUNT 25

// Application default config.

struct ConfigData {
    int soundVolume = 25;
    int musicVolume = 50;
    int fruitsType[GRID_SIZE][GRID_SIZE] = {
        {-1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1}
    };
    int ScorePoints = 0;
    int missStep = 0;
    int lostScores = 0;
    bool firstSrtart = true;
};
ConfigData* configData;

// Type of ui mode (for Google Wear 2.0 swipe back support).
#include "android/configuration.h"
static int uiModeType;

#include "Geometry.h"
#include "TimeManager.h"
#include "TweenManager.h"
#include "GraphicsManager.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "EventLoop.h"
#include "Activity.h"

// TODO:
// - redraw bonus
// - fix upper kill animation
// - Add bonus kill
// - Bonus random
// - New logo
// - Music pause
// - .. Refactoring to C++ best practics

// Android entry point.
void android_main(android_app* app) {
    LOG_INFO("--- Let's go full native! ---");
    LOG_INFO("Build date: %s %s", GetBuildDate(), __TIME__);
    srand(time(NULL));
    // Toggle fullscreen.
    ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_FULLSCREEN, AWINDOW_FLAG_FULLSCREEN);
    // Application details provided by Android.
    application = app;
    // Get ui type.
    AConfiguration* configuration = AConfiguration_new();
    AConfiguration_fromAssetManager(configuration, application->activity->assetManager);
    uiModeType = AConfiguration_getUiModeType(configuration);
    AConfiguration_delete(configuration);
    // Starts the game loop.
    EventLoop* eventLoop = new EventLoop();
    eventLoop->run(new Activity);
    // Finalize game.
    SAFE_DELETE(eventLoop);
    SAFE_DELETE(configData);
    LOG_INFO("--- Bye! ---");
}
