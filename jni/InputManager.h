#ifndef __INPUTMANAGER_H__
#define __INPUTMANAGER_H__

#include <math.h>
#include <android_native_app_glue.h>

#include "EventLoop.h"
#include "GraphicsManager.h"

//#define INPUTMANAGER_LOG_EVENTS // for input debug

struct Location {
	Location(): x(0.0f), y(0.0f) {};
	Location(float X, float Y): x(X), y(Y) {};
	float x;
	float y;
};

class Sensor {
public:
	Sensor(EventLoop& pEventLoop, int32_t pSensorType):
		mEventLoop(pEventLoop),
		mSensor(NULL),
		mSensorType(pSensorType) {
	}
	status enable() {
		if (mEventLoop.mEnabled) {
			mSensor = ASensorManager_getDefaultSensor(mEventLoop.mSensorManager, mSensorType);
			if (mSensor != NULL) {
				if (ASensorEventQueue_enableSensor(mEventLoop.mSensorEventQueue, mSensor) < 0) {
					goto ERROR;
				}
				const char* name = ASensor_getName(mSensor);
				const char* vendor = ASensor_getVendor(mSensor);
				float resolution = ASensor_getResolution(mSensor);
				int32_t minDelay = ASensor_getMinDelay(mSensor);
				if (ASensorEventQueue_setEventRate(mEventLoop.mSensorEventQueue, mSensor, minDelay) < 0) {
					goto ERROR;
				}
				LOG_INFO("Activating sensor:");
				LOG_INFO("Name       : %s", name);
				LOG_INFO("Vendor     : %s", vendor);
				LOG_INFO("Resolution : %f", resolution);
				LOG_INFO("Min Delay  : %d", minDelay);
			} else {
				LOG_ERROR("No sensor type %d", mSensorType);
			}
		}
		return STATUS_OK;
	ERROR:
		LOG_ERROR("Error while activating sensor.");
		disable();
		return STATUS_ERROR;
	}
	status disable() {
		if ((mEventLoop.mEnabled) && (mSensor != NULL)) {
			if (ASensorEventQueue_disableSensor(mEventLoop.mSensorEventQueue, mSensor) < 0) {
				goto ERROR;
			}
			mSensor = NULL;
		}
		return STATUS_OK;
	ERROR:
		LOG_ERROR("Error while deactivating sensor.");
		return STATUS_ERROR;
	}
private:
	EventLoop& mEventLoop;
	const ASensor* mSensor;
	int32_t mSensorType;
};

class InputManager: public InputHandler {
public:
	InputManager(android_app* pApplication, Sensor* pAccelerometer, GraphicsManager* pGraphicsManager) :
			mApplication(pApplication),
			mDirectionX(0.0f), mDirectionY(0.0f),
			mDownX(0.0f), mDownY(0.0f),
			mRefPoint(NULL), 
			mGraphicsManager(pGraphicsManager),
			mAccelerometer(pAccelerometer) {
		LOG_INFO("Creating InputManager");
	}
	~InputManager() {
		LOG_INFO("Destructing InputManager");
	}
	float getHorizontal() { return mDirectionX; }
	float getVertical() { return mDirectionY; }
	float getDownX() { return mDownX; }
	float getDownY() { return mDownY; }
	void setRefPoint(Location* pTouchReference) {
		mRefPoint = pTouchReference;
	}
	status start() {
		LOG_INFO("Starting InputManager");
		// Activates sensors
		mAccelerometer->enable();		
		mDirectionX = 0.0f,
		mDirectionY = 0.0f;
		mScaleFactor = float(mGraphicsManager->getRenderWidth()) / float(mGraphicsManager->getScreenWidth());		
		return STATUS_OK;
	}
	status update() {
		// Clears previous state.
		return STATUS_OK;
	};
	void stop() {
		LOG_INFO("Stopping InputManager");
		// It is important to disable sensors to save battery.
		mAccelerometer->disable();
	}
public:
	bool onTouchEvent(AInputEvent* pEvent) {
#ifdef INPUTMANAGER_LOG_EVENTS
		LOG_DEBUG("AMotionEvent_getAction=%d", AMotionEvent_getAction(pEvent));
		LOG_DEBUG("AMotionEvent_getFlags=%d", AMotionEvent_getFlags(pEvent));
		LOG_DEBUG("AMotionEvent_getMetaState=%d", AMotionEvent_getMetaState(pEvent));
		LOG_DEBUG("AMotionEvent_getEdgeFlags=%d", AMotionEvent_getEdgeFlags(pEvent));
		LOG_DEBUG("AMotionEvent_getDownTime=%lld", AMotionEvent_getDownTime(pEvent));
		LOG_DEBUG("AMotionEvent_getEventTime=%lld", AMotionEvent_getEventTime(pEvent));
		LOG_DEBUG("AMotionEvent_getXOffset=%f", AMotionEvent_getXOffset(pEvent));
		LOG_DEBUG("AMotionEvent_getYOffset=%f", AMotionEvent_getYOffset(pEvent));
		LOG_DEBUG("AMotionEvent_getXPrecision=%f", AMotionEvent_getXPrecision(pEvent));
		LOG_DEBUG("AMotionEvent_getYPrecision=%f", AMotionEvent_getYPrecision(pEvent));
		LOG_DEBUG("AMotionEvent_getPointerCount=%d", AMotionEvent_getPointerCount(pEvent));
		LOG_DEBUG("AMotionEvent_getRawX=%f", AMotionEvent_getRawX(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getRawY=%f", AMotionEvent_getRawY(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getX=%f", AMotionEvent_getX(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getY=%f", AMotionEvent_getY(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getPressure=%f", AMotionEvent_getPressure(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getSize=%f", AMotionEvent_getSize(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getOrientation=%f", AMotionEvent_getOrientation(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getTouchMajor=%f", AMotionEvent_getTouchMajor(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getTouchMinor=%f", AMotionEvent_getTouchMinor(pEvent, 0));
#endif
		const float TOUCH_MAX_RANGE = 65.0f; // In pixels.
		if (mRefPoint != NULL) {
			if (AMotionEvent_getAction(pEvent) == AMOTION_EVENT_ACTION_MOVE) {
				float x = AMotionEvent_getX(pEvent, 0) * mScaleFactor;
				float y = (float(mGraphicsManager->getScreenHeight()) - AMotionEvent_getY(pEvent, 0)) * mScaleFactor;
				// Needs a conversion to proper coordinates
				// (origin at bottom/left). Only moveY needs it.
				float moveX = x - mRefPoint->x;
				float moveY = y - mRefPoint->y;
				float lMoveRange = sqrt((moveX * moveX) + (moveY * moveY));
				if (lMoveRange > TOUCH_MAX_RANGE) {
					float lCropFactor = TOUCH_MAX_RANGE / lMoveRange;
					moveX *= lCropFactor;
					moveY *= lCropFactor;
				}
				mDirectionX = moveX / TOUCH_MAX_RANGE;
				mDirectionY   = moveY / TOUCH_MAX_RANGE;
			} else {
				mDirectionX = 0.0f;
				mDirectionY = 0.0f;
			}
		}
		if (AMotionEvent_getAction(pEvent) == AMOTION_EVENT_ACTION_DOWN) {
			// downPointerId = AMotionEvent_getPointerId(pEvent, 0); !!!!!!!!!!!!!!
			mDownX = AMotionEvent_getX(pEvent, 0) * mScaleFactor;
			mDownY = ((float)mGraphicsManager->getScreenHeight() - AMotionEvent_getY(pEvent, 0)) * mScaleFactor;
		}
		if (AMotionEvent_getAction(pEvent) == AMOTION_EVENT_ACTION_UP) {
			mDownX = -1.0f;
			mDownY = -1.0f;
		}		
		return true;
	}
	bool onKeyboardEvent(AInputEvent* pEvent) {
#ifdef INPUTMANAGER_LOG_EVENTS
		LOG_DEBUG("AKeyEvent_getAction=%d", AKeyEvent_getAction(pEvent));
		LOG_DEBUG("AKeyEvent_getFlags=%d", AKeyEvent_getFlags(pEvent));
		LOG_DEBUG("AKeyEvent_getKeyCode=%d", AKeyEvent_getKeyCode(pEvent));
		LOG_DEBUG("AKeyEvent_getScanCode=%d", AKeyEvent_getScanCode(pEvent));
		LOG_DEBUG("AKeyEvent_getMetaState=%d", AKeyEvent_getMetaState(pEvent));
		LOG_DEBUG("AKeyEvent_getRepeatCount=%d", AKeyEvent_getRepeatCount(pEvent));
		LOG_DEBUG("AKeyEvent_getDownTime=%lld", AKeyEvent_getDownTime(pEvent));
		LOG_DEBUG("AKeyEvent_getEventTime=%lld", AKeyEvent_getEventTime(pEvent));
#endif
		const float ORTHOGONAL_MOVE = 1.0f;
		if (AKeyEvent_getAction(pEvent) == AKEY_EVENT_ACTION_DOWN) {
			switch (AKeyEvent_getKeyCode(pEvent)) {
			case AKEYCODE_DPAD_LEFT:
				mDirectionX = -ORTHOGONAL_MOVE;
				break;
			case AKEYCODE_DPAD_RIGHT:
				mDirectionX = ORTHOGONAL_MOVE;
				break;
			case AKEYCODE_DPAD_DOWN:
				mDirectionY = -ORTHOGONAL_MOVE;
				break;
			case AKEYCODE_DPAD_UP:
				mDirectionY = ORTHOGONAL_MOVE;
				break;
			case AKEYCODE_BACK:
				return false;
			}
		} else {
			switch (AKeyEvent_getKeyCode(pEvent)) {
			case AKEYCODE_DPAD_LEFT:
			case AKEYCODE_DPAD_RIGHT:
				mDirectionX = 0.0f;
				break;
			case AKEYCODE_DPAD_DOWN:
			case AKEYCODE_DPAD_UP:
				mDirectionY = 0.0f;
				break;
			case AKEYCODE_BACK:
				return false;
			}
		}
		return true;
	}
	bool onTrackballEvent(AInputEvent* pEvent) {
#ifdef INPUTMANAGER_LOG_EVENTS
		LOG_DEBUG("AMotionEvent_getAction=%d", AMotionEvent_getAction(pEvent));
		LOG_DEBUG("AMotionEvent_getFlags=%d", AMotionEvent_getFlags(pEvent));
		LOG_DEBUG("AMotionEvent_getMetaState=%d", AMotionEvent_getMetaState(pEvent));
		LOG_DEBUG("AMotionEvent_getEdgeFlags=%d", AMotionEvent_getEdgeFlags(pEvent));
		LOG_DEBUG("AMotionEvent_getDownTime=%lld", AMotionEvent_getDownTime(pEvent));
		LOG_DEBUG("AMotionEvent_getEventTime=%lld", AMotionEvent_getEventTime(pEvent));
		LOG_DEBUG("AMotionEvent_getXOffset=%f", AMotionEvent_getXOffset(pEvent));
		LOG_DEBUG("AMotionEvent_getYOffset=%f", AMotionEvent_getYOffset(pEvent));
		LOG_DEBUG("AMotionEvent_getXPrecision=%f", AMotionEvent_getXPrecision(pEvent));
		LOG_DEBUG("AMotionEvent_getYPrecision=%f", AMotionEvent_getYPrecision(pEvent));
		LOG_DEBUG("AMotionEvent_getPointerCount=%d", AMotionEvent_getPointerCount(pEvent));
		LOG_DEBUG("AMotionEvent_getRawX=%f", AMotionEvent_getRawX(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getRawY=%f", AMotionEvent_getRawY(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getX=%f", AMotionEvent_getX(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getY=%f", AMotionEvent_getY(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getPressure=%f", AMotionEvent_getPressure(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getSize=%f", AMotionEvent_getSize(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getOrientation=%f", AMotionEvent_getOrientation(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getTouchMajor=%f", AMotionEvent_getTouchMajor(pEvent, 0));
		LOG_DEBUG("AMotionEvent_getTouchMinor=%f", AMotionEvent_getTouchMinor(pEvent, 0));
#endif
		const float ORTHOGONAL_MOVE = 1.0f;
		const float DIAGONAL_MOVE   = 0.707f;
		const float THRESHOLD       = (1/100.0f);
		if (AMotionEvent_getAction(pEvent) == AMOTION_EVENT_ACTION_MOVE) {
			float directionX = AMotionEvent_getX(pEvent, 0);
			float directionY = AMotionEvent_getY(pEvent, 0);
			float horizontal, vertical;
			if (directionX < -THRESHOLD) {
				if (directionY < -THRESHOLD) {
					horizontal = -DIAGONAL_MOVE;
					vertical   =  DIAGONAL_MOVE;
				} else if (directionY > THRESHOLD) {
					horizontal = -DIAGONAL_MOVE;
					vertical   = -DIAGONAL_MOVE;
				} else {
					horizontal = -ORTHOGONAL_MOVE;
					vertical   = 0.0f;
				}
			} else if (directionX > THRESHOLD) {
				if (directionY < -THRESHOLD) {
					horizontal = DIAGONAL_MOVE;
					vertical   = DIAGONAL_MOVE;
				} else if (directionY > THRESHOLD) {
					horizontal =  DIAGONAL_MOVE;
					vertical   = -DIAGONAL_MOVE;
				} else {
					horizontal = ORTHOGONAL_MOVE;
					vertical   = 0.0f;
				}
			} else if (directionY < -THRESHOLD) {
				horizontal = 0.0f;
				vertical   = ORTHOGONAL_MOVE;
			} else if (directionY > THRESHOLD) {
				horizontal = 0.0f;
				vertical   = -ORTHOGONAL_MOVE;
			}
			// Ends movement if there is a counter movement.
			if ((horizontal < 0.0f) && (mDirectionX > 0.0f)) {
				mDirectionX = 0.0f;
			} else if ((horizontal > 0.0f) && (mDirectionX < 0.0f)) {
				mDirectionX = 0.0f;
			} else {
				mDirectionX = horizontal;
			}
			if ((vertical < 0.0f) && (mDirectionY > 0.0f)) {
				mDirectionY = 0.0f;
			} else if ((vertical > 0.0f) && (mDirectionY < 0.0f)) {
				mDirectionY = 0.0f;
			} else {
				mDirectionY = vertical;
			}
		} else {
			mDirectionX = 0.0f;
			mDirectionY = 0.0f;
		}
		return true;
	}
	bool onAccelerometerEvent(ASensorEvent* pEvent) {
#ifdef INPUTMANAGER_LOG_EVENTS
		LOG_DEBUG("ASensorEvent=%d", pEvent->version);
		LOG_DEBUG("ASensorEvent=%d", pEvent->sensor);
		LOG_DEBUG("ASensorEvent=%lld", pEvent->timestamp);
		LOG_DEBUG("ASensorEvent=%d", pEvent->type);
		LOG_DEBUG("ASensorEvent=%f,%f,%f,%d", pEvent->acceleration.x, pEvent->acceleration.y, pEvent->acceleration.z, pEvent->acceleration.status);
#endif
		const float GRAVITY = ASENSOR_STANDARD_GRAVITY / 2.0f;
		const float MIN_X = -1.0f;
		const float MAX_X =  1.0f;
		const float MIN_Y =  0.0f;
		const float MAX_Y =  2.0f;
		const float CENTER_X = (MAX_X + MIN_X) / 2.0f;
		const float CENTER_Y = (MAX_Y + MIN_Y) / 2.0f;
		// Roll tilt.
		// Use y-axis instead for landscape oriented devices:
		// float rawHorizontal = pEvent->vector.y / GRAVITY;
		float rawHorizontal = pEvent->vector.x / GRAVITY;
		if (rawHorizontal > MAX_X) {
			rawHorizontal = MAX_X;
		} else if (rawHorizontal < MIN_X) {
			rawHorizontal = MIN_X;
		}
		mDirectionX = CENTER_X - rawHorizontal;
		// Pitch tilt. Final value needs to be inversed.
		float rawVertical = pEvent->vector.z / GRAVITY;
		if (rawVertical > MAX_Y) {
			rawVertical = MAX_Y;
		} else if (rawVertical < MIN_Y) {
			rawVertical = MIN_Y;
		}
		mDirectionY = rawVertical - CENTER_Y;
		return true;
	}
private:
	android_app* mApplication;
	// Input values.
	float mDirectionX, mDirectionY;
	float mDownX, mDownY;
	// Reference point to evaluate touch distance.
	Location* mRefPoint;
	GraphicsManager* mGraphicsManager;
	float mScaleFactor;
	// Sensors.
	Sensor* mAccelerometer;
};

#endif
