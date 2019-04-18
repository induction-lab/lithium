#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <android_native_app_glue.h>
#include <android/window.h>
#include <android/input.h>
#include <android/sensor.h>

class ActivityHandler {
public:
    virtual ~ActivityHandler() {};
    virtual status onFirstStart() = 0;
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
    EventLoop() :
        isEnabled(false), isQuit(false), isFirstStart(true),
        activityHandler(NULL),
        sensorPollSource(),
		sensorManager(NULL),
        sensorEventQueue(NULL) {
        LOG_INFO("Creating Engine");
        application->userData = this;
        application->onAppCmd = callback_event;
        application->onInputEvent = callback_input;
		LOG_INFO("Create services");
		new TimeManager;	
		new InputManager;
		new GraphicsManager;
		new SoundManager;
	}
    void run(ActivityHandler* activity) {
        int32_t result;
        int32_t events;
        android_poll_source* source;
        // Makes sure native glue is not stripped by the linker.
        app_dummy();
        activityHandler = activity;
        LOG_INFO("Starting event loop");
        while (true) {
            // Event processing loop.
            while ((result = ALooper_pollAll(isEnabled ? 0 : -1, NULL, &events, (void**) &source)) >= 0) {
                // An event has to be processed.
                if (source != NULL) {
                    source->process(application, source);
                }
                // Application is getting destroyed.
                if (application->destroyRequested) {
                    LOG_INFO("Exiting event loop");
                    return;
                }
            }
            // Steps the application.
            if ((isEnabled) && (!isQuit)) {
                if (update() != STATUS_OK) {
                    isQuit = true;
                    ANativeActivity_finish(application->activity);
                }
            }
        }
    }
protected:
	status update() {
		// Updates services.
		TimeManager::getInstance().update();
		if (InputManager::getInstance().update() != STATUS_OK) return STATUS_ERROR;
		if (GraphicsManager::getInstance().update() != STATUS_OK) return STATUS_ERROR;
		if (activityHandler->onStep() != STATUS_OK) return STATUS_ERROR;
		return STATUS_OK;
	}
    void activate() {
        // Enables activity only if a window is available.
        if ((!isEnabled) && (application->window != NULL)) {
            // Registers sensor queue.
            sensorPollSource.id = LOOPER_ID_USER;
            sensorPollSource.app = application;
            sensorPollSource.process = callback_sensor;
            sensorManager = ASensorManager_getInstance();
            if (sensorManager != NULL) {
                sensorEventQueue = ASensorManager_createEventQueue(sensorManager, application->looper, LOOPER_ID_USER, NULL, &sensorPollSource);
                if (sensorEventQueue == NULL) goto ERROR;
            }
            activateAccelerometer();
            isQuit = false;
            isEnabled = true;
			// Starts services.
			LOG_INFO("Activating services");
			TimeManager::getInstance().start();
			if (InputManager::getInstance().start() != STATUS_OK) goto ERROR;
			if (GraphicsManager::getInstance().start() != STATUS_OK) goto ERROR;
			if (SoundManager::getInstance().start() != STATUS_OK) goto ERROR;
			if (isFirstStart) {
				if (activityHandler->onFirstStart() != STATUS_OK) goto ERROR;
				isFirstStart = false;
			}			
            if (activityHandler->onActivate() != STATUS_OK) goto ERROR;
			// Load resources.
			GraphicsManager::getInstance().loadResources();
			SoundManager::getInstance().loadResources();			
        }
        return;
ERROR:
        isQuit = true;
        deactivate();
        ANativeActivity_finish(application->activity);
    }
    void deactivate() {
        if (isEnabled) {
            deactivateAccelerometer();
            if (sensorEventQueue != NULL) {
                ASensorManager_destroyEventQueue(sensorManager, sensorEventQueue);
                sensorEventQueue = NULL;
            }
            sensorManager = NULL;
            activityHandler->onDeactivate();
			// Stop services.
			LOG_INFO("Deactivating services");
			GraphicsManager::getInstance().stop();
			InputManager::getInstance().stop();
			SoundManager::getInstance().stop();
			TimeManager::getInstance().stop();			
            isEnabled = false;
        }
    }
	
    void processAppEvent(int32_t command) {
        switch (command) {
        case APP_CMD_CONFIG_CHANGED:
			LOG_INFO("onConfigurationChanged");
            activityHandler->onConfigurationChanged();
            break;
        case APP_CMD_INIT_WINDOW:
			LOG_INFO("onCreateWindow");
            activityHandler->onCreateWindow();
            break;
        case APP_CMD_DESTROY:
			LOG_INFO("onDestroy");
            activityHandler->onDestroy();
            break;
        case APP_CMD_GAINED_FOCUS:
			LOG_INFO("onGainFocus");		
            activate();
            activityHandler->onGainFocus();
            break;
        case APP_CMD_LOST_FOCUS:
			LOG_INFO("onLostFocus");		
            activityHandler->onLostFocus();
            deactivate();
            break;
        case APP_CMD_LOW_MEMORY:
			LOG_INFO("onLowMemory");
            activityHandler->onLowMemory();
            break;
        case APP_CMD_PAUSE:
			LOG_INFO("onPause");		
            activityHandler->onPause();
            deactivate();
            break;
        case APP_CMD_RESUME:
			LOG_INFO("onResume");		
            activityHandler->onResume();
            break;
        case APP_CMD_SAVE_STATE:
			LOG_INFO("onSaveInstanceState");
            activityHandler->onSaveInstanceState(&application->savedState, &application->savedStateSize);
            break;
        case APP_CMD_START:
			LOG_INFO("onStart");
            activityHandler->onStart();
            break;
        case APP_CMD_STOP:
			LOG_INFO("onStop");
            activityHandler->onStop();
            break;
        case APP_CMD_TERM_WINDOW:
			LOG_INFO("onDestroyWindow");
            activityHandler->onDestroyWindow();
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
                return InputManager::getInstance().onTouchEvent(event);
                break;
            case AINPUT_SOURCE_TRACKBALL:
                return InputManager::getInstance().onTrackballEvent(event);
                break;
            }
            break;
        case AINPUT_EVENT_TYPE_KEY:
            return InputManager::getInstance().onKeyboardEvent(event);
            break;
        }
        return 0;
    }
    void processSensorEvent() {
		if (sensorEventQueue != NULL) {
			ASensorEvent event;			
			while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
				switch (event.type) {
				case ASENSOR_TYPE_ACCELEROMETER:
					InputManager::getInstance().onAccelerometerEvent(&event);
					break;
				}
			}
		}
    }
private:
    // Private callbacks handling events occuring in the thread loop.
    static void callback_event(android_app* application, int32_t command) {
        EventLoop& eventLoop = *(EventLoop*) application->userData;
        eventLoop.processAppEvent(command);
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
        accelerometer = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
        if (accelerometer != NULL) {
            if (ASensorEventQueue_enableSensor(sensorEventQueue, accelerometer) < 0) {
                LOG_ERROR("Could not enable accelerometer");
                return;
            }
            const char* name = ASensor_getName(accelerometer);
            const char* vendor = ASensor_getVendor(accelerometer);
            float resolution = ASensor_getResolution(accelerometer);
            int32_t minDelay = ASensor_getMinDelay(accelerometer);
            LOG_INFO("Activating accelerometer:");
            LOG_INFO("Name       : %s", name);
            LOG_INFO("Vendor     : %s", vendor);
            LOG_INFO("Resolution : %f", resolution);
            LOG_INFO("Min Delay  : %d", minDelay);
            // Uses maximum refresh rate.
            if (ASensorEventQueue_setEventRate(sensorEventQueue, accelerometer, minDelay) < 0) {
                LOG_ERROR("Could not set accelerometer rate");
            }
        } else {
            LOG_ERROR("No accelerometer found");
        }
    }
    void deactivateAccelerometer() {
        if (accelerometer != NULL) {
            if (ASensorEventQueue_disableSensor(sensorEventQueue, accelerometer) < 0) {
                LOG_ERROR("Error while deactivating accelerometer.");
            }
            accelerometer = NULL;
        }
    }
private:
    // Saves application state when application is active/paused.
    bool isEnabled;
    // Indicates if the event handler wants to exit.
    bool isQuit;
	// Indicates if application is launched for the first time.
	bool isFirstStart;
    // Activity event observer.
    ActivityHandler* activityHandler;
    // Sensors
    ASensorManager* sensorManager;
    ASensorEventQueue* sensorEventQueue;
    android_poll_source sensorPollSource;
    const ASensor* accelerometer;
};

#endif
