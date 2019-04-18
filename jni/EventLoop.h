#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <android_native_app_glue.h>
#include <android/window.h>
#include <android/input.h>
#include <android/sensor.h>

class InputHandler {
public:
    virtual ~InputHandler() {};
    virtual bool onTouchEvent(AInputEvent* event) = 0;
    virtual bool onKeyboardEvent(AInputEvent* event) = 0;
    virtual bool onTrackballEvent(AInputEvent* event) = 0;
    virtual bool onAccelerometerEvent(ASensorEvent* event) = 0;
};

class ActivityHandler {
public:
    virtual ~ActivityHandler() {};
    virtual status onActivate() = 0;
    virtual void onDeactivate() = 0;
    virtual status onStep() = 0;
    virtual void onStart() {};
    virtual void onResume() {};
    virtual void onPause() {};
    virtual void onStop() {};
    virtual void onDestroy() {};
    virtual void onSaveInstanceState(void** data, size_t* size) {};
    virtual void onConfigurationChanged() {};
    virtual void onLowMemory() {};
    virtual void onCreateWindow() {};
    virtual void onDestroyWindow() {};
    virtual void onGainFocus() {};
    virtual void onLostFocus() {};
};

class EventLoop {
public:
    EventLoop(android_app* application) :
        mEnabled(false), mQuit(false),
        mApplication(application),
        mActivityHandler(NULL),
		mInputHandler(NULL),
        mSensorPollSource(),
		mSensorManager(NULL),
        mSensorEventQueue(NULL) {
        mApplication->userData = this;
        mApplication->onAppCmd = callback_event;
        mApplication->onInputEvent = callback_input;
    }
    void run(ActivityHandler* activityHandler, InputHandler* inputHandler) {
        int32_t result;
        int32_t events;
        android_poll_source* source;
        // Makes sure native glue is not stripped by the linker.
        app_dummy();
        mActivityHandler = activityHandler;
        mInputHandler    = inputHandler;
        LOG_INFO("Starting event loop");
        while (true) {
            // Event processing loop.
            while ((result = ALooper_pollAll(mEnabled ? 0 : -1, NULL, &events, (void**) &source)) >= 0) {
                // An event has to be processed.
                if (source != NULL) {
                    source->process(mApplication, source);
                }
                // Application is getting destroyed.
                if (mApplication->destroyRequested) {
                    LOG_INFO("Exiting event loop");
                    return;
                }
            }
            // Steps the application.
            if ((mEnabled) && (!mQuit)) {
                if (mActivityHandler->onStep() != STATUS_OK) {
                    mQuit = true;
                    ANativeActivity_finish(mApplication->activity);
                }
            }
        }
    }
protected:
    void activate() {
        // Enables activity only if a window is available.
        if ((!mEnabled) && (mApplication->window != NULL)) {
            // Registers sensor queue.
            mSensorPollSource.id = LOOPER_ID_USER;
            mSensorPollSource.app = mApplication;
            mSensorPollSource.process = callback_sensor;
            mSensorManager = ASensorManager_getInstance();
            if (mSensorManager != NULL) {
                mSensorEventQueue = ASensorManager_createEventQueue(mSensorManager, mApplication->looper, LOOPER_ID_USER, NULL, &mSensorPollSource);
                if (mSensorEventQueue == NULL) goto ERROR;
            }
            activateAccelerometer();
            mQuit = false;
            mEnabled = true;
            if (mActivityHandler->onActivate() != STATUS_OK) {
                goto ERROR;
            }
        }
        return;
ERROR:
        mQuit = true;
        deactivate();
        ANativeActivity_finish(mApplication->activity);
    }
    void deactivate() {
        if (mEnabled) {
            deactivateAccelerometer();
            if (mSensorEventQueue != NULL) {
                ASensorManager_destroyEventQueue(mSensorManager, mSensorEventQueue);
                mSensorEventQueue = NULL;
            }
            mSensorManager = NULL;
            mActivityHandler->onDeactivate();
            mEnabled = false;
        }
    }
    void processApevent(int32_t command) {
        switch (command) {
        case APP_CMD_CONFIG_CHANGED:
            mActivityHandler->onConfigurationChanged();
            break;
        case APP_CMD_INIT_WINDOW:
            mActivityHandler->onCreateWindow();
            break;
        case APP_CMD_DESTROY:
            mActivityHandler->onDestroy();
            break;
        case APP_CMD_GAINED_FOCUS:
            activate();
            mActivityHandler->onGainFocus();
            break;
        case APP_CMD_LOST_FOCUS:
            mActivityHandler->onLostFocus();
            deactivate();
            break;
        case APP_CMD_LOW_MEMORY:
            mActivityHandler->onLowMemory();
            break;
        case APP_CMD_PAUSE:
            mActivityHandler->onPause();
            deactivate();
            break;
        case APP_CMD_RESUME:
            mActivityHandler->onResume();
            break;
        case APP_CMD_SAVE_STATE:
            mActivityHandler->onSaveInstanceState(&mApplication->savedState, &mApplication->savedStateSize);
            break;
        case APP_CMD_START:
            mActivityHandler->onStart();
            break;
        case APP_CMD_STOP:
            mActivityHandler->onStop();
            break;
        case APP_CMD_TERM_WINDOW:
            mActivityHandler->onDestroyWindow();
            deactivate();
            break;
        default:
            break;
        }
    }
    int32_t processInputEvent(AInputEvent* event) {
        int32_t eventType = AInputEvent_getType(event);
        switch (eventType) {
        case AINPUT_EVENT_TYPE_MOTION:
            switch (AInputEvent_getSource(event)) {
            case AINPUT_SOURCE_TOUCHSCREEN:
                return mInputHandler->onTouchEvent(event);
                break;
            case AINPUT_SOURCE_TRACKBALL:
                return mInputHandler->onTrackballEvent(event);
                break;
            }
            break;
        case AINPUT_EVENT_TYPE_KEY:
            return mInputHandler->onKeyboardEvent(event);
            break;
        }
        return 0;
    }
    void processSensorEvent() {
		if (mSensorEventQueue != NULL) {
			ASensorEvent event;			
			while (ASensorEventQueue_getEvents(mSensorEventQueue, &event, 1) > 0) {
				switch (event.type) {
				case ASENSOR_TYPE_ACCELEROMETER:
					mInputHandler->onAccelerometerEvent(&event);
					break;
				}
			}
		}
    }
private:
    // Private callbacks handling events occuring in the thread loop.
    static void callback_event(android_app* application, int32_t command) {
        EventLoop& eventLoop = *(EventLoop*) application->userData;
        eventLoop.processApevent(command);
    }
    static int32_t callback_input(android_app* application, AInputEvent* event) {
        EventLoop& eventLoop = *(EventLoop*) application->userData;
        return eventLoop.processInputEvent(event);
    }
    static void callback_sensor(android_app* application, android_poll_source* source) {
        EventLoop& eventLoop = *(EventLoop*) application->userData;
        eventLoop.processSensorEvent();
    }
private:
    void activateAccelerometer() {
        mAccelerometer = ASensorManager_getDefaultSensor(mSensorManager, ASENSOR_TYPE_ACCELEROMETER);
        if (mAccelerometer != NULL) {
            if (ASensorEventQueue_enableSensor(mSensorEventQueue, mAccelerometer) < 0) {
                LOG_ERROR("Could not enable accelerometer");
                return;
            }
            const char* name = ASensor_getName(mAccelerometer);
            const char* vendor = ASensor_getVendor(mAccelerometer);
            float resolution = ASensor_getResolution(mAccelerometer);
            int32_t minDelay = ASensor_getMinDelay(mAccelerometer);
            LOG_INFO("Activating accelerometer:");
            LOG_INFO("Name       : %s", name);
            LOG_INFO("Vendor     : %s", vendor);
            LOG_INFO("Resolution : %f", resolution);
            LOG_INFO("Min Delay  : %d", minDelay);
            // Uses maximum refresh rate.
            if (ASensorEventQueue_setEventRate(mSensorEventQueue, mAccelerometer, minDelay) < 0) {
                LOG_ERROR("Could not set accelerometer rate");
            }
        } else {
            LOG_ERROR("No accelerometer found");
        }
    }
    void deactivateAccelerometer() {
        if (mAccelerometer != NULL) {
            if (ASensorEventQueue_disableSensor(mSensorEventQueue, mAccelerometer) < 0) {
                LOG_ERROR("Error while deactivating accelerometer.");
            }
            mAccelerometer = NULL;
        }
    }
private:
    // Saves application state when application is active/paused.
    bool mEnabled;
    // Indicates if the event handler wants to exit.
    bool mQuit;
    // Application details provided by Android.
    android_app* mApplication;
    // Activity event observer.
    ActivityHandler* mActivityHandler;
    // Input event observer.
    InputHandler* mInputHandler;
    // Sensors
    ASensorManager* mSensorManager;
    ASensorEventQueue* mSensorEventQueue;
    android_poll_source mSensorPollSource;
    const ASensor* mAccelerometer;
};

#endif
