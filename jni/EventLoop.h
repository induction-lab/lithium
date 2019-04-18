#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <android_native_app_glue.h>
#include <android/window.h>
#include <android/input.h>
#include <android/sensor.h>

class InputHandler {
public: 
	virtual ~InputHandler() {};
	virtual bool onTouchEvent(AInputEvent* pEvent) = 0;
	virtual bool onKeyboardEvent(AInputEvent* pEvent) = 0;
	virtual bool onTrackballEvent(AInputEvent* pEvent) = 0;
	virtual bool onAccelerometerEvent(ASensorEvent* pEvent) = 0;
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
    virtual void onSaveInstanceState(void** pData, size_t* pSize) {};
    virtual void onConfigurationChanged() {};
    virtual void onLowMemory() {};
    virtual void onCreateWindow() {};
    virtual void onDestroyWindow() {};
    virtual void onGainFocus() {};
    virtual void onLostFocus() {};
};

class EventLoop {
public:
	EventLoop(android_app* pApplication) :
			mEnabled(false), mQuit(false),
			mApplication(pApplication),
			mActivityHandler(NULL), mInputHandler(NULL),
			mSensorPolsource(), mSensorManager(NULL),
			mSensorEventQueue(NULL) {
		mApplication->userData = this;
		mApplication->onAppCmd = callback_event;
		mApplication->onInputEvent = callback_input;
	}
	void run(ActivityHandler* pActivityHandler, InputHandler* pInputHandler) {
		int32_t lResult;
		int32_t lEvents;
		android_poll_source* source;
		// Makes sure native glue is not stripped by the linker.
		app_dummy();
		mActivityHandler = pActivityHandler;
		mInputHandler    = pInputHandler;
		// Global step loop.
		LOG_INFO("Starting event loop");
		while (true) {
			// Event processing loop.
			while ((lResult = ALooper_pollAll(mEnabled ? 0 : -1, NULL, &lEvents, (void**) &source)) >= 0) {
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
			mSensorPolsource.id = LOOPER_ID_USER;
			mSensorPolsource.app = mApplication;
			mSensorPolsource.process = callback_sensor;
			mSensorManager = ASensorManager_getInstance();
			if (mSensorManager != NULL) {
				mSensorEventQueue = ASensorManager_createEventQueue(mSensorManager, mApplication->looper, LOOPER_ID_USER, NULL, &mSensorPolsource);
				if (mSensorEventQueue == NULL) goto ERROR;
			}
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
			mActivityHandler->onDeactivate();
			mEnabled = false;
			if (mSensorEventQueue != NULL) {
				ASensorManager_destroyEventQueue(mSensorManager, mSensorEventQueue);
				mSensorEventQueue = NULL;
			}
			mSensorManager = NULL;
		}
	}
	void processAppEvent(int32_t pCommand) {
		switch (pCommand) {
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
	int32_t processInputEvent(AInputEvent* pEvent) {
		int32_t lEventType = AInputEvent_getType(pEvent);
		switch (lEventType) {
		case AINPUT_EVENT_TYPE_MOTION:
			switch (AInputEvent_getSource(pEvent)) {
			case AINPUT_SOURCE_TOUCHSCREEN:
				return mInputHandler->onTouchEvent(pEvent);
				break;
			case AINPUT_SOURCE_TRACKBALL:
				return mInputHandler->onTrackballEvent(pEvent);
				break;
			}
			break;
		case AINPUT_EVENT_TYPE_KEY:
			return mInputHandler->onKeyboardEvent(pEvent);
			break;
		}
		return 0;
	}
	void processSensorEvent() {
		ASensorEvent lEvent;
		while (ASensorEventQueue_getEvents(mSensorEventQueue, &lEvent, 1) > 0) {
			switch (lEvent.type) {
			case ASENSOR_TYPE_ACCELEROMETER:
				mInputHandler->onAccelerometerEvent(&lEvent);
				break;
			}
		}
	}
private:
	friend class Sensor;
	// Private callbacks handling events occuring in the thread loop.
	static void callback_event(android_app* pApplication, int32_t pCommand) {
		EventLoop& lEventLoop = *(EventLoop*) pApplication->userData;
		lEventLoop.processAppEvent(pCommand);
	}
	static int32_t callback_input(android_app* pApplication, AInputEvent* pEvent) {
		EventLoop& lEventLoop = *(EventLoop*) pApplication->userData;
		return lEventLoop.processInputEvent(pEvent);
	}
	static void callback_sensor(android_app* pApplication, android_poll_source* pSource) {
		EventLoop& lEventLoop = *(EventLoop*) pApplication->userData;
		lEventLoop.processSensorEvent();
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
	// Event queue.
	ASensorManager* mSensorManager;
	ASensorEventQueue* mSensorEventQueue;
	android_poll_source mSensorPolsource;
};

#endif
