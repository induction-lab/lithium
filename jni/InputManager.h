#ifndef __INPUTMANAGER_H__
#define __INPUTMANAGER_H__

#include <math.h>
#include <android_native_app_glue.h>

#include <vector>
#include <algorithm>

#include "EventLoop.h"
#include "TimeManager.h"
#include "GraphicsManager.h"

//#define INPUTMANAGER_LOG_EVENTS // for input debug

struct Location {
    Location(): x(0.0f), y(0.0f) {};
    Location(float X, float Y): x(X), y(Y) {};
    float x;
    float y;
};

struct TouchPointerData {
    size_t pointerId;
    bool pressed;
    double time;
    int x;
    int y;
};

#define GESTURE_TAP_DURATION_MAX			200 / 100
#define GESTURE_SWIPE_DURATION_MAX      	400 / 100
#define GESTURE_LONG_TAP_DURATION_MIN   	GESTURE_TAP_DURATION_MAX
#define GESTURE_DRAG_START_DURATION_MIN		GESTURE_LONG_TAP_DURATION_MIN
#define GESTURE_DRAG_DISTANCE_MIN    		30
#define GESTURE_SWIPE_DISTANCE_MIN      	50
#define GESTURE_PINCH_DISTANCE_MIN    		GESTURE_DRAG_DISTANCE_MIN

static const bool MULTI_TOUTCH = true;
static const int SWIPE_DIRECTION_UP		= 1 << 0;	// The up direction for a swipe event.
static const int SWIPE_DIRECTION_DOWN	= 1 << 1;	// The down direction for a swipe event.
static const int SWIPE_DIRECTION_LEFT	= 1 << 2;	// The left direction for a swipe event.
static const int SWIPE_DIRECTION_RIGHT	= 1 << 3;	// The right direction for a swipe event.

class InputListener {
public:
    InputListener(void);
    virtual ~InputListener(void);
    virtual void touchPressEvent(int x, int y, size_t pointerId) {};
    virtual void touchReleaseEvent(int x, int y, size_t pointerId) {};
    virtual void touchMoveEvent(int x, int y, size_t pointerId) {};
    virtual void gestureDragEvent(int x, int y) {};
    virtual void gestureDropEvent(int x, int y) {};
    virtual void gestureSwipeEvent(int x, int y, int direction) {};
    virtual void gestureTapEvent(int x, int y) {};
    virtual void gestureLongTapEvent(int x, int y, float time) {};
    virtual void gesturePinchEvent(int x, int y, float scale) {};
};

class InputManager: public InputHandler {
public:
    InputManager(android_app* application) :
        mApplication(application) {
        LOG_INFO("Creating InputManager");
        mInstance = this;
    }
    ~InputManager() {
        LOG_INFO("Destructing InputManager");
        mInstance = 0;
    }
    static InputManager* getPtr() {
        return mInstance;
    }
    status start() {
        LOG_INFO("Starting InputManager");
        mScaleFactor = (float)GraphicsManager::getPtr()->getRenderWidth() / (float)GraphicsManager::getPtr()->getScreenWidth();
        return STATUS_OK;
    }
    status update() {
        // Clears previous state.
        return STATUS_OK;
    };
    void stop() {
        LOG_INFO("Stopping InputManager");
    }
public:
    void registerListener(InputListener *listener) {
        mListeners.push_back(listener);
    }
    void unregisterListener(InputListener *listener) {
        mListeners.erase(std::find(mListeners.begin(), mListeners.end(), listener));
    }
    bool onTouchEvent(AInputEvent* event) {
#ifdef INPUTMANAGER_LOG_EVENTS
        LOG_DEBUG("AMotionEvent_getAction=%d", AMotionEvent_getAction(event));
        LOG_DEBUG("AMotionEvent_getFlags=%d", AMotionEvent_getFlags(event));
        LOG_DEBUG("AMotionEvent_getMetaState=%d", AMotionEvent_getMetaState(event));
        LOG_DEBUG("AMotionEvent_getEdgeFlags=%d", AMotionEvent_getEdgeFlags(event));
        LOG_DEBUG("AMotionEvent_getDownTime=%lld", AMotionEvent_getDownTime(event));
        LOG_DEBUG("AMotionEvent_getEventTime=%lld", AMotionEvent_getEventTime(event));
        LOG_DEBUG("AMotionEvent_getXOffset=%f", AMotionEvent_getXOffset(event));
        LOG_DEBUG("AMotionEvent_getYOffset=%f", AMotionEvent_getYOffset(event));
        LOG_DEBUG("AMotionEvent_getXPrecision=%f", AMotionEvent_getXPrecision(event));
        LOG_DEBUG("AMotionEvent_getYPrecision=%f", AMotionEvent_getYPrecision(event));
        LOG_DEBUG("AMotionEvent_getPointerCount=%d", AMotionEvent_getPointerCount(event));
        LOG_DEBUG("AMotionEvent_getRawX=%f", AMotionEvent_getRawX(event, 0));
        LOG_DEBUG("AMotionEvent_getRawY=%f", AMotionEvent_getRawY(event, 0));
        LOG_DEBUG("AMotionEvent_getX=%f", AMotionEvent_getX(event, 0));
        LOG_DEBUG("AMotionEvent_getY=%f", AMotionEvent_getY(event, 0));
        LOG_DEBUG("AMotionEvent_getPressure=%f", AMotionEvent_getPressure(event, 0));
        LOG_DEBUG("AMotionEvent_getSize=%f", AMotionEvent_getSize(event, 0));
        LOG_DEBUG("AMotionEvent_getOrientation=%f", AMotionEvent_getOrientation(event, 0));
        LOG_DEBUG("AMotionEvent_getTouchMajor=%f", AMotionEvent_getTouchMajor(event, 0));
        LOG_DEBUG("AMotionEvent_getTouchMinor=%f", AMotionEvent_getTouchMinor(event, 0));
#endif
        int32_t action = AMotionEvent_getAction(event);
        size_t pointerIndex;
        size_t pointerId;
        size_t pointerCount;
        int x, y;
		bool gestureDetected;
        float time = TimeManager::getPtr()->getTime();		
        switch (action & AMOTION_EVENT_ACTION_MASK) {
        case AMOTION_EVENT_ACTION_DOWN:
            pointerId = AMotionEvent_getPointerId(event, 0);
            x = AMotionEvent_getX(event, 0);
            y = AMotionEvent_getY(event, 0);
            // Gesture handling
			mPointer0.pressed = true;
			mPointer0.time = time;
			mPointer0.pointerId = pointerId;
			mPointer0.x = x;
			mPointer0.y = y;
			mGesturePointer0CurrentPosition = mGesturePointer0LastPosition = std::pair<int, int>(x, y);
            // Primary pointer down.
            onTouchPressEvent(x, y, pointerId);
            mPrimaryTouchId = pointerId;
			break;
        case AMOTION_EVENT_ACTION_UP:
            pointerId = AMotionEvent_getPointerId(event, 0);
            x = AMotionEvent_getX(event, 0);
            y = AMotionEvent_getY(event, 0);
            // Gestures
            gestureDetected = false;
            if (MULTI_TOUTCH || mPrimaryTouchId == pointerId) {
                onTouchReleaseEvent(x, y, pointerId);
            }
            if (mPointer0.pressed &&  mPointer0.pointerId == pointerId) {
                int deltaX = x - mPointer0.x;
                int deltaY = y - mPointer0.y;
                // Test for drop
                if (mGesturePinching) {
                    mGesturePinching = false;
                    gestureDetected = true;
                } else if (mGestureDraging) {
                    onGestureDropEvent(x, y);
                    gestureDetected = true;
                    mGestureDraging = false;
                }
                // Test for swipe
                else if (time - mPointer0.time < GESTURE_SWIPE_DURATION_MAX && (abs(deltaX) > GESTURE_SWIPE_DISTANCE_MIN || abs(deltaY) > GESTURE_SWIPE_DISTANCE_MIN) ) {
                    int direction = 0;
                    if ( abs(deltaX) > abs(deltaY) ) {
                        if (deltaX > 0) direction = SWIPE_DIRECTION_RIGHT;
                        else if (deltaX < 0) direction = SWIPE_DIRECTION_LEFT;
                    } else {
                        if (deltaY > 0) direction = SWIPE_DIRECTION_DOWN;
                        else if (deltaY < 0) SWIPE_DIRECTION_UP;
                    }
                    onGestureSwipeEvent(x, y, direction);
                    gestureDetected = true;
                }
                // Test for tap
                else if(time - mPointer0.time < GESTURE_TAP_DURATION_MAX) {
                    onGestureTapEvent(x, y);
                    gestureDetected = true;
                }
                // Test for long tap
                else if(time - mPointer0.time >= GESTURE_LONG_TAP_DURATION_MIN) {
                    onGestureLongTapEvent(x, y, time - mPointer0.time);
                    gestureDetected = true;
                }
            }
            mPointer0.pressed = false;
            mPrimaryTouchId = -1;
			break;
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            pointerId = AMotionEvent_getPointerId(event, pointerIndex);
            x = AMotionEvent_getX(event, pointerIndex);
            y = AMotionEvent_getY(event, pointerIndex);
            // Gesture handling
			mPointer1.pressed = true;
			mPointer1.time = time;
			mPointer1.pointerId = pointerId;
			mPointer1.x = x;
			mPointer1.y = y;
			mGesturePointer1CurrentPosition = mGesturePointer1LastPosition = std::pair<int, int>(x, y);
            // Non-primary pointer down.
            if (MULTI_TOUTCH) {
                onTouchPressEvent(AMotionEvent_getX(event, pointerIndex), AMotionEvent_getY(event, pointerIndex), pointerId);
            }
			break;
        case AMOTION_EVENT_ACTION_POINTER_UP:
            pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            pointerId = AMotionEvent_getPointerId(event, pointerIndex);
            x = AMotionEvent_getX(event, pointerIndex);
            y = AMotionEvent_getY(event, pointerIndex);
            gestureDetected = false;
            if (!gestureDetected && (MULTI_TOUTCH || mPrimaryTouchId == pointerId) ) {
                onTouchReleaseEvent(AMotionEvent_getX(event, pointerIndex), AMotionEvent_getY(event, pointerIndex), pointerId);
            }
            if (mPointer1.pressed &&  mPointer1.pointerId == pointerId) {
                int deltaX = x - mPointer1.x;
                int deltaY = y - mPointer1.y;
                if (mGesturePinching) {
                    mGesturePinching = false;
                    gestureDetected = true;
                }
                // Test for swipe
                else if (time - mPointer1.time < GESTURE_SWIPE_DURATION_MAX && (abs(deltaX) > GESTURE_SWIPE_DISTANCE_MIN || abs(deltaY) > GESTURE_SWIPE_DISTANCE_MIN) ) {
                    int direction = 0;
                    if (deltaX > 0) direction |= SWIPE_DIRECTION_RIGHT;
                    else if (deltaX < 0) direction |= SWIPE_DIRECTION_LEFT;
                    if (deltaY > 0) direction |= SWIPE_DIRECTION_DOWN;
                    else if (deltaY < 0) direction |= SWIPE_DIRECTION_UP;
                    onGestureSwipeEvent(x, y, direction);
                    gestureDetected = true;
                } else if(time - mPointer1.time < GESTURE_TAP_DURATION_MAX) {
                    onGestureTapEvent(x, y);
                    gestureDetected = true;
                } else if(time - mPointer1.time >= GESTURE_LONG_TAP_DURATION_MIN) {
                    onGestureLongTapEvent(x, y, time - mPointer1.time);
                    gestureDetected = true;
                }
            }
            mPointer1.pressed = false;
            if (mPrimaryTouchId == pointerId) mPrimaryTouchId = -1;
			break;
        case AMOTION_EVENT_ACTION_MOVE:
            // ACTION_MOVE events are batched, unlike the other events.
            pointerCount = AMotionEvent_getPointerCount(event);
            for (size_t i = 0; i < pointerCount; ++i) {
                pointerId = AMotionEvent_getPointerId(event, i);
                x = AMotionEvent_getX(event, i);
                y = AMotionEvent_getY(event, i);
                gestureDetected = false;
                if (mPointer0.pressed) {
                    //The two pointers are pressed and the event was done by one of it
                    if (mPointer1.pressed && (pointerId == mPointer0.pointerId || pointerId == mPointer1.pointerId)) {
                        if (mPointer0.pointerId == mPointer1.pointerId) {
                            mGesturePinching = false;
                            break;
                        }
                        int pointer0Distance, pointer1Distance;
                        if (mPointer0.pointerId == pointerId) {
                            mGesturePointer0LastPosition = mGesturePointer0CurrentPosition;
                            mGesturePointer0CurrentPosition = std::pair<int, int>(x, y);
                            mGesturePointer0Delta = sqrt(pow((float)(x - mPointer0.x), 2) + pow((float)(y - mPointer0.y), 2));
                        } else {
                            mGesturePointer1LastPosition = mGesturePointer1CurrentPosition;
                            mGesturePointer1CurrentPosition = std::pair<int, int>(x, y);
                            mGesturePointer1Delta = sqrt(pow((float)(x - mPointer1.x), 2) + pow((float)(y - mPointer1.y), 2));
                        }
                        if (!mGesturePinching && mGesturePointer0Delta >= GESTURE_PINCH_DISTANCE_MIN && mGesturePointer1Delta >= GESTURE_PINCH_DISTANCE_MIN) {
                            mGesturePinching = true;
                            mGesturePinchCentroid = std::pair<int, int>((mPointer0.x + mPointer1.x) / 2, (mPointer0.y + mPointer1.y) / 2);
                        }
                        if (mGesturePinching) {
                            int currentDistancePointer0, currentDistancePointer1;
                            int lastDistancePointer0, lastDistancePointer1;
                            float scale;

                            currentDistancePointer0 = sqrt(pow((float)(mGesturePinchCentroid.first - mGesturePointer0CurrentPosition.first), 2) + pow((float)(mGesturePinchCentroid.second - mGesturePointer0CurrentPosition.second), 2));
                            lastDistancePointer0 = sqrt(pow((float)(mGesturePinchCentroid.first - mGesturePointer0LastPosition.first), 2) + pow((float)(mGesturePinchCentroid.second - mGesturePointer0LastPosition.second), 2));
                            currentDistancePointer1 = sqrt(pow((float)(mGesturePinchCentroid.first - mGesturePointer1CurrentPosition.first), 2) + pow((float)(mGesturePinchCentroid.second - mGesturePointer1CurrentPosition.second), 2));
                            lastDistancePointer1 = sqrt(pow((float)(mGesturePinchCentroid.first - mGesturePointer1LastPosition.first), 2) + pow((float)(mGesturePinchCentroid.second - mGesturePointer1LastPosition.second), 2));
                            if (pointerId == mPointer0.pointerId) scale = ((float) currentDistancePointer0) / ((float) lastDistancePointer0);
                            else scale = ((float) currentDistancePointer1) / ((float) lastDistancePointer1);
                            if (((currentDistancePointer0 >= lastDistancePointer0) && (currentDistancePointer1 >= lastDistancePointer1)) || ((currentDistancePointer0 <= lastDistancePointer0) && (currentDistancePointer1 <= lastDistancePointer1))) {
                                onGesturePinchEvent(mGesturePinchCentroid.first, mGesturePinchCentroid.second, scale);
                                gestureDetected = true;
                            } else mGesturePinching = false;
                        }
                    }
                    // Only the primary pointer is done and the event was done by it
                    else if (!gestureDetected && pointerId == mPointer0.pointerId) {
                        //Test for drag
                        int delta = sqrt(pow((float)(x - mPointer0.x), 2) + pow((float)(y - mPointer0.y), 2));
                        if (mGestureDraging || (time - mPointer0.time >= GESTURE_DRAG_START_DURATION_MIN) && (delta >= GESTURE_DRAG_DISTANCE_MIN)) {
                            onGestureDragEvent(x, y);
                            mGestureDraging = true;
                            gestureDetected = true;
                        }

                    }
                }
                if (!gestureDetected && (MULTI_TOUTCH || mPrimaryTouchId == pointerId)) {
                    onTouchMoveEvent(AMotionEvent_getX(event, i), AMotionEvent_getY(event, i), pointerId);
                }
            }
			break;
        }
    }
    bool onKeyboardEvent(AInputEvent* event) {
#ifdef INPUTMANAGER_LOG_EVENTS
        LOG_DEBUG("AKeyEvent_getAction=%d", AKeyEvent_getAction(event));
        LOG_DEBUG("AKeyEvent_getFlags=%d", AKeyEvent_getFlags(event));
        LOG_DEBUG("AKeyEvent_getKeyCode=%d", AKeyEvent_getKeyCode(event));
        LOG_DEBUG("AKeyEvent_getScanCode=%d", AKeyEvent_getScanCode(event));
        LOG_DEBUG("AKeyEvent_getMetaState=%d", AKeyEvent_getMetaState(event));
        LOG_DEBUG("AKeyEvent_getRepeatCount=%d", AKeyEvent_getRepeatCount(event));
        LOG_DEBUG("AKeyEvent_getDownTime=%lld", AKeyEvent_getDownTime(event));
        LOG_DEBUG("AKeyEvent_getEventTime=%lld", AKeyEvent_getEventTime(event));
#endif
        return true;
    }
    bool onTrackballEvent(AInputEvent* event) {
#ifdef INPUTMANAGER_LOG_EVENTS
        LOG_DEBUG("AMotionEvent_getAction=%d", AMotionEvent_getAction(event));
        LOG_DEBUG("AMotionEvent_getFlags=%d", AMotionEvent_getFlags(event));
        LOG_DEBUG("AMotionEvent_getMetaState=%d", AMotionEvent_getMetaState(event));
        LOG_DEBUG("AMotionEvent_getEdgeFlags=%d", AMotionEvent_getEdgeFlags(event));
        LOG_DEBUG("AMotionEvent_getDownTime=%lld", AMotionEvent_getDownTime(event));
        LOG_DEBUG("AMotionEvent_getEventTime=%lld", AMotionEvent_getEventTime(event));
        LOG_DEBUG("AMotionEvent_getXOffset=%f", AMotionEvent_getXOffset(event));
        LOG_DEBUG("AMotionEvent_getYOffset=%f", AMotionEvent_getYOffset(event));
        LOG_DEBUG("AMotionEvent_getXPrecision=%f", AMotionEvent_getXPrecision(event));
        LOG_DEBUG("AMotionEvent_getYPrecision=%f", AMotionEvent_getYPrecision(event));
        LOG_DEBUG("AMotionEvent_getPointerCount=%d", AMotionEvent_getPointerCount(event));
        LOG_DEBUG("AMotionEvent_getRawX=%f", AMotionEvent_getRawX(event, 0));
        LOG_DEBUG("AMotionEvent_getRawY=%f", AMotionEvent_getRawY(event, 0));
        LOG_DEBUG("AMotionEvent_getX=%f", AMotionEvent_getX(event, 0));
        LOG_DEBUG("AMotionEvent_getY=%f", AMotionEvent_getY(event, 0));
        LOG_DEBUG("AMotionEvent_getPressure=%f", AMotionEvent_getPressure(event, 0));
        LOG_DEBUG("AMotionEvent_getSize=%f", AMotionEvent_getSize(event, 0));
        LOG_DEBUG("AMotionEvent_getOrientation=%f", AMotionEvent_getOrientation(event, 0));
        LOG_DEBUG("AMotionEvent_getTouchMajor=%f", AMotionEvent_getTouchMajor(event, 0));
        LOG_DEBUG("AMotionEvent_getTouchMinor=%f", AMotionEvent_getTouchMinor(event, 0));
#endif
        return true;
    }
    bool onAccelerometerEvent(ASensorEvent* event) {
#ifdef INPUTMANAGER_LOG_EVENTS
        LOG_DEBUG("ASensorEvent=%d", event->version);
        LOG_DEBUG("ASensorEvent=%d", event->sensor);
        LOG_DEBUG("ASensorEvent=%lld", event->timestamp);
        LOG_DEBUG("ASensorEvent=%d", event->type);
        LOG_DEBUG("ASensorEvent=%f,%f,%f,%d", event->acceleration.x, event->acceleration.y, event->acceleration.z, event->acceleration.status);
#endif
        return true;
    }
private:
    void onTouchPressEvent(int x, int y, size_t pointerId) {
		x = x * mScaleFactor;
		y = ((float)GraphicsManager::getPtr()->getScreenHeight() - y) * mScaleFactor;
		for (std::vector<InputListener*>::iterator it = mListeners.begin(); it < mListeners.end(); ++it) {
            (*it)->touchPressEvent(x, y, pointerId);
        }
    };
    void onTouchReleaseEvent(int x, int y, size_t pointerId) {
		x = x * mScaleFactor;
		y = ((float)GraphicsManager::getPtr()->getScreenHeight() - y) * mScaleFactor;
		for (std::vector<InputListener*>::iterator it = mListeners.begin(); it < mListeners.end(); ++it) {
            (*it)->touchReleaseEvent(x, y, pointerId);
        }
    };
    void onTouchMoveEvent(int x, int y, size_t pointerId) {
		x = x * mScaleFactor;
		y = ((float)GraphicsManager::getPtr()->getScreenHeight() - y) * mScaleFactor;		
        for (std::vector<InputListener*>::iterator it = mListeners.begin(); it < mListeners.end(); ++it) {
            (*it)->touchMoveEvent(x, y, pointerId);
        }
    };
    void onGestureDragEvent(int x, int y) {
		x = x * mScaleFactor;
		y = ((float)GraphicsManager::getPtr()->getScreenHeight() - y) * mScaleFactor;		
        for (std::vector<InputListener*>::iterator it = mListeners.begin(); it < mListeners.end(); ++it) {
            (*it)->gestureDragEvent(x, y);
        }
    };
    void onGestureDropEvent(int x, int y) {
		x = x * mScaleFactor;
		y = ((float)GraphicsManager::getPtr()->getScreenHeight() - y) * mScaleFactor;		
        for (std::vector<InputListener*>::iterator it = mListeners.begin(); it < mListeners.end(); ++it) {
            (*it)->gestureDropEvent(x, y);
        }
    };
    void onGestureSwipeEvent(int x, int y, int direction) {
		x = x * mScaleFactor;
		y = ((float)GraphicsManager::getPtr()->getScreenHeight() - y) * mScaleFactor;		
        for (std::vector<InputListener*>::iterator it = mListeners.begin(); it < mListeners.end(); ++it) {
            (*it)->gestureSwipeEvent(x, y, direction);
        }
    };
    void onGestureTapEvent(int x, int y) {
		x = x * mScaleFactor;
		y = ((float)GraphicsManager::getPtr()->getScreenHeight() - y) * mScaleFactor;		
        for (std::vector<InputListener*>::iterator it = mListeners.begin(); it < mListeners.end(); ++it) {
            (*it)->gestureTapEvent(x, y);
        }
    };
    void onGestureLongTapEvent(int x, int y, float time) {
		x = x * mScaleFactor;
		y = ((float)GraphicsManager::getPtr()->getScreenHeight() - y) * mScaleFactor;		
        for (std::vector<InputListener*>::iterator it = mListeners.begin(); it < mListeners.end(); ++it) {
            (*it)->gestureLongTapEvent(x, y, time);
        }
    };
    void onGesturePinchEvent(int x, int y, float scale) {
		scale = scale * mScaleFactor;
		x = x * mScaleFactor;
		y = ((float)GraphicsManager::getPtr()->getScreenHeight() - y) * mScaleFactor;		
        for (std::vector<InputListener*>::iterator it = mListeners.begin(); it < mListeners.end(); ++it) {
            (*it)->gesturePinchEvent(x, y, scale);
        }
    };
private:
    static InputManager* mInstance;
    android_app* mApplication;
    float mScaleFactor;
    int mPrimaryTouchId = -1;
    TouchPointerData mPointer0;
    TouchPointerData mPointer1;
    bool mGestureDraging = false;
    bool mGesturePinching = false;
    std::pair<int, int> mGesturePointer0LastPosition;
    std::pair<int, int> mGesturePointer1LastPosition;
    std::pair<int, int> mGesturePointer0CurrentPosition;
    std::pair<int, int> mGesturePointer1CurrentPosition;
    std::pair<int, int> mGesturePinchCentroid;
    int mGesturePointer0Delta, mGesturePointer1Delta;
    std::vector<InputListener*> mListeners;
};

InputManager* InputManager::mInstance = 0;

InputListener::InputListener(void) {
    InputManager::getPtr()->registerListener(this);
}

InputListener::~InputListener(void) {
    InputManager::getPtr()->unregisterListener(this);
}

#endif
