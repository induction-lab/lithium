#ifndef __INPUTMANAGER_H__
#define __INPUTMANAGER_H__

#include <math.h>
#include <android/input.h>
#include <android/sensor.h>

#include <vector>

#include "Singleton.h"

// #define INPUTMANAGER_LOG_EVENTS
// #define INPUTMANAGER_SENSORS_EVENTS
// #define INPUTMANAGER_LOG_SENSOR_EVENTS

struct TouchPointerData {
    size_t pointerId;
    bool pressed;
    double time;
    int x;
    int y;
};

// Defines a touch screen event.
class Touch {
public:
    static const unsigned int MAX_TOUCH_POINTS = 10; // maximum simultaneous touch points supported
    enum TouchEvent {
        TOUCH_DOWN,
        TOUCH_UP,
        TOUCH_MOVE
    };
private:
    Touch() {};
};

#define GESTURE_TAP_DURATION_MAX            200 * 0.001
#define GESTURE_SWIPE_DURATION_MAX          400 * 0.001
#define GESTURE_LONG_TAP_DURATION_MIN       GESTURE_TAP_DURATION_MAX
#define GESTURE_DRAG_START_DURATION_MIN     GESTURE_LONG_TAP_DURATION_MIN
#define GESTURE_DRAG_DISTANCE_MIN           15
#define GESTURE_SWIPE_DISTANCE_MIN          50
#define GESTURE_PINCH_DISTANCE_MIN          GESTURE_DRAG_DISTANCE_MIN

static const bool MULTI_TOUTCH = true;
static const int SWIPE_DIRECTION_UP     = 1 << 0;   // the up direction for a swipe event
static const int SWIPE_DIRECTION_DOWN   = 1 << 1;   // the down direction for a swipe event
static const int SWIPE_DIRECTION_LEFT   = 1 << 2;   // the left direction for a swipe event
static const int SWIPE_DIRECTION_RIGHT  = 1 << 3;   // the right direction for a swipe event

class InputListener {
public:
    InputListener(void);
    virtual ~InputListener(void);
    virtual void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {};
    virtual int gestureDragEvent(int x, int y) { return 0; };
    virtual int gestureDropEvent(int x, int y) { return 0; };
    virtual int gestureSwipeEvent(int x, int y, int direction) { return 0; };
    virtual int gestureTapEvent(int x, int y) { return 0; };
    virtual int gestureLongTapEvent(int x, int y, float time) { return 0; };
    virtual int gesturePinchEvent(int x, int y, float scale) { return 0; };
    virtual int backEvent() { return 0; };
    virtual void keyDownEvent(int keyCode) {};
    virtual void keyUpEvent(int keyCode) {};
};

class InputManager: public Singleton<InputManager> {
public:
    InputManager():
        primaryTouchId(-1),
        gesturePointer0Delta(0),
        gesturePointer1Delta(0),
        gestureDraging(false),
        gesturePinching(false) {
        LOG_INFO("Creating InputManager.");
        //
    };
    ~InputManager() {
        LOG_INFO("Destructing InputManager.");
    };
    status start() {
        LOG_INFO("Starting InputManager.");
        return STATUS_OK;
    };
    status update() {
        // Clears previous state.
        return STATUS_OK;
    };
    void stop() {
        LOG_INFO("Stopping InputManager.");
    };
public:
    void registerListener(InputListener *listener) {
        // LOG_DEBUG("Register InputListener %d.", listeners.size() + 1);
        listeners.push_back(listener);
    };
    void unregisterListener(InputListener *listener) {
        // LOG_DEBUG("Unregister InputListener %d.", listeners.size());
        listeners.erase(std::find(listeners.begin(), listeners.end(), listener));
    };
    int32_t onTouchEvent(AInputEvent* event) {
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
#endif // INPUTMANAGER_LOG_EVENTS
        int32_t action = AMotionEvent_getAction(event);
        size_t pointerIndex;
        size_t pointerId;
        size_t pointerCount;
        int x, y;
        bool gestureDetected;
        float time = TimeManager::getInstance()->getTime();
        switch (action & AMOTION_EVENT_ACTION_MASK) {
        case AMOTION_EVENT_ACTION_DOWN:
            pointerId = AMotionEvent_getPointerId(event, 0);
            x = AMotionEvent_getX(event, 0);
            y = AMotionEvent_getY(event, 0);
            // Gesture handling.
            pointer0.pressed = true;
            pointer0.time = time;
            pointer0.pointerId = pointerId;
            pointer0.x = x;
            pointer0.y = y;
            gesturePointer0CurrentPosition = gesturePointer0LastPosition = std::pair<int, int>(x, y);
            // Primary pointer down.
            onTouchEvent(Touch::TOUCH_DOWN, x, y, pointerId);
            primaryTouchId = pointerId;
            break;
        case AMOTION_EVENT_ACTION_UP:
            pointerId = AMotionEvent_getPointerId(event, 0);
            x = AMotionEvent_getX(event, 0);
            y = AMotionEvent_getY(event, 0);
            // Gestures.
            gestureDetected = false;
            if (pointer0.pressed && pointer0.pointerId == pointerId) {
                int deltaX = x - pointer0.x;
                int deltaY = y - pointer0.y;
                // Test for drop.
                if (gesturePinching) {
                    gesturePinching = false;
                    gestureDetected = true;
                } else if (gestureDraging) {
                    gestureDetected = true;
                    gestureDraging = false;
                    onGestureDropEvent(x, y);
                }
                // Test for swipe.
                else if (time - pointer0.time < GESTURE_SWIPE_DURATION_MAX && (abs(deltaX) > GESTURE_SWIPE_DISTANCE_MIN || abs(deltaY) > GESTURE_SWIPE_DISTANCE_MIN)) {
                    int direction = 0;
                    if (abs(deltaX) > abs(deltaY)) {
                        if (deltaX > 0) direction = SWIPE_DIRECTION_RIGHT;
                        else if (deltaX < 0) direction = SWIPE_DIRECTION_LEFT;
                    } else {
                        if (deltaY > 0) direction = SWIPE_DIRECTION_DOWN;
                        else if (deltaY < 0) direction = SWIPE_DIRECTION_UP;
                    }
                    gestureDetected = true;
                    onGestureSwipeEvent(x, y, direction);
                }
                // Test for tap.
                else if(time - pointer0.time < GESTURE_TAP_DURATION_MAX) {
                    gestureDetected = true;
                    onGestureTapEvent(x, y);
                }
                // Test for long tap.
                else if(time - pointer0.time >= GESTURE_LONG_TAP_DURATION_MIN) {
                    gestureDetected = true;
                    onGestureLongTapEvent(x, y, time - pointer0.time);
                }
            }
            pointer0.pressed = false;
            primaryTouchId = -1;
            if (MULTI_TOUTCH || primaryTouchId == pointerId) {
                onTouchEvent(Touch::TOUCH_UP, x, y, pointerId);
            }
            break;
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            pointerId = AMotionEvent_getPointerId(event, pointerIndex);
            x = AMotionEvent_getX(event, pointerIndex);
            y = AMotionEvent_getY(event, pointerIndex);
            // Gesture handling.
            pointer1.pressed = true;
            pointer1.time = time;
            pointer1.pointerId = pointerId;
            pointer1.x = x;
            pointer1.y = y;
            gesturePointer1CurrentPosition = gesturePointer1LastPosition = std::pair<int, int>(x, y);
            // Non-primary pointer down.
            if (MULTI_TOUTCH) {
                onTouchEvent(Touch::TOUCH_DOWN, AMotionEvent_getX(event, pointerIndex), AMotionEvent_getY(event, pointerIndex), pointerId);
            }
            break;
        case AMOTION_EVENT_ACTION_POINTER_UP:
            pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            pointerId = AMotionEvent_getPointerId(event, pointerIndex);
            x = AMotionEvent_getX(event, pointerIndex);
            y = AMotionEvent_getY(event, pointerIndex);
            gestureDetected = false;
            if (pointer1.pressed &&  pointer1.pointerId == pointerId) {
                int deltaX = x - pointer1.x;
                int deltaY = y - pointer1.y;
                if (gesturePinching) {
                    gesturePinching = false;
                    gestureDetected = true;
                }
                // Test for swipe.
                else if (time - pointer1.time < GESTURE_SWIPE_DURATION_MAX && (abs(deltaX) > GESTURE_SWIPE_DISTANCE_MIN || abs(deltaY) > GESTURE_SWIPE_DISTANCE_MIN) ) {
                    int direction = 0;
                    if (deltaX > 0) direction |= SWIPE_DIRECTION_RIGHT;
                    else if (deltaX < 0) direction |= SWIPE_DIRECTION_LEFT;
                    if (deltaY > 0) direction |= SWIPE_DIRECTION_DOWN;
                    else if (deltaY < 0) direction |= SWIPE_DIRECTION_UP;
                    onGestureSwipeEvent(x, y, direction);
                    gestureDetected = true;
                }
                // Test for tap.
                else if(time - pointer1.time < GESTURE_TAP_DURATION_MAX) {
                    onGestureTapEvent(x, y);
                    gestureDetected = true;
                }
                // Test for long tap.
                else if(time - pointer1.time >= GESTURE_LONG_TAP_DURATION_MIN) {
                    onGestureLongTapEvent(x, y, time - pointer1.time);
                    gestureDetected = true;
                }
            }
            pointer1.pressed = false;
            if (primaryTouchId == pointerId) primaryTouchId = -1;
            if (MULTI_TOUTCH || primaryTouchId == pointerId) {
                onTouchEvent(Touch::TOUCH_UP, AMotionEvent_getX(event, pointerIndex), AMotionEvent_getY(event, pointerIndex), pointerId);
            }
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            // ACTION_MOVE events are batched, unlike the other events.
            pointerCount = AMotionEvent_getPointerCount(event);
            for (size_t i = 0; i < pointerCount; ++i) {
                pointerId = AMotionEvent_getPointerId(event, i);
                x = AMotionEvent_getX(event, i);
                y = AMotionEvent_getY(event, i);
                gestureDetected = false;
                if (pointer0.pressed) {
                    // The two pointers are pressed and the event was done by one of it.
                    if (pointer1.pressed && (pointerId == pointer0.pointerId || pointerId == pointer1.pointerId)) {
                        if (pointer0.pointerId == pointer1.pointerId) {
                            gesturePinching = false;
                            break;
                        }
                        int pointer0Distance, pointer1Distance;
                        if (pointer0.pointerId == pointerId) {
                            gesturePointer0LastPosition = gesturePointer0CurrentPosition;
                            gesturePointer0CurrentPosition = std::pair<int, int>(x, y);
                            gesturePointer0Delta = sqrt(pow((float)(x - pointer0.x), 2) + pow((float)(y - pointer0.y), 2));
                        } else {
                            gesturePointer1LastPosition = gesturePointer1CurrentPosition;
                            gesturePointer1CurrentPosition = std::pair<int, int>(x, y);
                            gesturePointer1Delta = sqrt(pow((float)(x - pointer1.x), 2) + pow((float)(y - pointer1.y), 2));
                        }
                        if (!gesturePinching && gesturePointer0Delta >= GESTURE_PINCH_DISTANCE_MIN && gesturePointer1Delta >= GESTURE_PINCH_DISTANCE_MIN) {
                            gesturePinching = true;
                            gesturePinchCentroid = std::pair<int, int>((pointer0.x + pointer1.x) / 2, (pointer0.y + pointer1.y) / 2);
                        }
                        if (gesturePinching) {
                            int currentDistancePointer0, currentDistancePointer1;
                            int lastDistancePointer0, lastDistancePointer1;
                            float scale;
                            currentDistancePointer0 = sqrt(pow((float)(gesturePinchCentroid.first - gesturePointer0CurrentPosition.first), 2) + pow((float)(gesturePinchCentroid.second - gesturePointer0CurrentPosition.second), 2));
                            lastDistancePointer0 = sqrt(pow((float)(gesturePinchCentroid.first - gesturePointer0LastPosition.first), 2) + pow((float)(gesturePinchCentroid.second - gesturePointer0LastPosition.second), 2));
                            currentDistancePointer1 = sqrt(pow((float)(gesturePinchCentroid.first - gesturePointer1CurrentPosition.first), 2) + pow((float)(gesturePinchCentroid.second - gesturePointer1CurrentPosition.second), 2));
                            lastDistancePointer1 = sqrt(pow((float)(gesturePinchCentroid.first - gesturePointer1LastPosition.first), 2) + pow((float)(gesturePinchCentroid.second - gesturePointer1LastPosition.second), 2));
                            if (pointerId == pointer0.pointerId) scale = ((float) currentDistancePointer0) / ((float) lastDistancePointer0);
                            else scale = ((float) currentDistancePointer1) / ((float) lastDistancePointer1);
                            if (((currentDistancePointer0 >= lastDistancePointer0) && (currentDistancePointer1 >= lastDistancePointer1)) || ((currentDistancePointer0 <= lastDistancePointer0) && (currentDistancePointer1 <= lastDistancePointer1))) {
                                onGesturePinchEvent(gesturePinchCentroid.first, gesturePinchCentroid.second, scale);
                                gestureDetected = true;
                            } else gesturePinching = false;
                        }
                    }
                    // Only the primary pointer is done and the event was done by it.
                    else if (!gestureDetected && pointerId == pointer0.pointerId) {
                        // Test for drag.
                        int delta = sqrt(pow((float)(x - pointer0.x), 2) + pow((float)(y - pointer0.y), 2));
                        if (gestureDraging || ((time - pointer0.time >= GESTURE_DRAG_START_DURATION_MIN) && (delta >= GESTURE_DRAG_DISTANCE_MIN))) {
                            onGestureDragEvent(x, y);
                            gestureDraging = true;
                            gestureDetected = true;
                        }
                    }
                }
                if (MULTI_TOUTCH || primaryTouchId == pointerId) {
                    onTouchEvent(Touch::TOUCH_MOVE, AMotionEvent_getX(event, i), AMotionEvent_getY(event, i), pointerId);
                }
            }
            break;
        }
        return 0;
    };
    int32_t onKeyboardEvent(AInputEvent* event) {
#ifdef INPUTMANAGER_LOG_EVENTS
        LOG_DEBUG("AKeyEvent_getAction=%d", AKeyEvent_getAction(event));
        LOG_DEBUG("AKeyEvent_getFlags=%d", AKeyEvent_getFlags(event));
        LOG_DEBUG("AKeyEvent_getKeyCode=%d", AKeyEvent_getKeyCode(event));
        LOG_DEBUG("AKeyEvent_getScanCode=%d", AKeyEvent_getScanCode(event));
        LOG_DEBUG("AKeyEvent_getMetaState=%d", AKeyEvent_getMetaState(event));
        LOG_DEBUG("AKeyEvent_getRepeatCount=%d", AKeyEvent_getRepeatCount(event));
        LOG_DEBUG("AKeyEvent_getDownTime=%lld", AKeyEvent_getDownTime(event));
        LOG_DEBUG("AKeyEvent_getEventTime=%lld", AKeyEvent_getEventTime(event));
#endif // INPUTMANAGER_LOG_EVENTS
        int32_t action = AMotionEvent_getAction(event);
        int32_t keyCode = AKeyEvent_getKeyCode(event);
        switch (action & AMOTION_EVENT_ACTION_MASK) {
            case AKEY_EVENT_ACTION_DOWN:
                if (keyCode == AKEYCODE_BACK) {
                    onBackEvent();
                    return 1;
                } else onKeyDownEvent(keyCode);
                break;
            case AKEY_EVENT_ACTION_UP:
                onKeyUpEvent(keyCode);
                break;
        }
        return 0;
    };
    int32_t onTrackballEvent(AInputEvent* event) {
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
#endif // INPUTMANAGER_LOG_EVENTS
        return 0;
    };
    int32_t onAccelerometerEvent(ASensorEvent* event) {
    #ifdef INPUTMANAGER_LOG_SENSOR_EVENTS
        LOG_DEBUG("ASensorEvent=%d", event->version);
        LOG_DEBUG("ASensorEvent=%d", event->sensor);
        LOG_DEBUG("ASensorEvent=%lld", event->timestamp);
        LOG_DEBUG("ASensorEvent=%d", event->type);
        LOG_DEBUG("ASensorEvent=%f,%f,%f,%d", event->acceleration.x, event->acceleration.y, event->acceleration.z, event->acceleration.status);
#endif // INPUTMANAGER_LOG_SENSOR_EVENTS
        // Not implemented yet.
        return 0;
    };
private:
    // Send event to listeners ...
    void onTouchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        for (std::vector<InputListener*>::const_reverse_iterator  it = listeners.rbegin(); it < listeners.rend(); ++it) {
            (*it)->touchEvent(event, x, y, pointerId);
        }
    };
    void onGestureDragEvent(int x, int y) {
        for (std::vector<InputListener*>::const_reverse_iterator  it = listeners.rbegin(); it < listeners.rend(); ++it) {
            if ((*it)->gestureDragEvent(x, y)) break;
        }
    };
    void onGestureDropEvent(int x, int y) {
        for (std::vector<InputListener*>::const_reverse_iterator  it = listeners.rbegin(); it < listeners.rend(); ++it) {
            if ((*it)->gestureDropEvent(x, y)) break;
        }
    };
    void onGestureSwipeEvent(int x, int y, int direction) {
        for (std::vector<InputListener*>::const_reverse_iterator  it = listeners.rbegin(); it < listeners.rend(); ++it) {
            if ((*it)->gestureSwipeEvent(x, y, direction)) break;
        }
    };
    void onGestureTapEvent(int x, int y) {
        for (std::vector<InputListener*>::const_reverse_iterator  it = listeners.rbegin(); it < listeners.rend(); ++it) {
            if ((*it)->gestureTapEvent(x, y)) break;
        }
    };
    void onGestureLongTapEvent(int x, int y, float time) {
        for (std::vector<InputListener*>::const_reverse_iterator  it = listeners.rbegin(); it < listeners.rend(); ++it) {
            if ((*it)->gestureLongTapEvent(x, y, time)) break;
        }
    };
    void onGesturePinchEvent(int x, int y, float scale) {
        for (std::vector<InputListener*>::const_reverse_iterator  it = listeners.rbegin(); it < listeners.rend(); ++it) {
            if ((*it)->gesturePinchEvent(x, y, scale)) break;
        }
    };
    void onBackEvent() {
        for (std::vector<InputListener*>::const_reverse_iterator  it = listeners.rbegin(); it < listeners.rend(); ++it) {
            if ((*it)->backEvent()) break;
        }
    };
    void onKeyDownEvent(int keyCode) {
        for (std::vector<InputListener*>::const_reverse_iterator  it = listeners.rbegin(); it < listeners.rend(); ++it) {
            (*it)->keyDownEvent(keyCode);
        }
    };
    void onKeyUpEvent(int keyCode) {
        for (std::vector<InputListener*>::const_reverse_iterator  it = listeners.rbegin(); it < listeners.rend(); ++it) {
            (*it)->keyUpEvent(keyCode);
        }
    }; 
private:
    int primaryTouchId;
    TouchPointerData pointer0;
    TouchPointerData pointer1;
    bool gestureDraging;
    bool gesturePinching;
    std::pair<int, int> gesturePointer0LastPosition;
    std::pair<int, int> gesturePointer1LastPosition;
    std::pair<int, int> gesturePointer0CurrentPosition;
    std::pair<int, int> gesturePointer1CurrentPosition;
    std::pair<int, int> gesturePinchCentroid;
    int gesturePointer0Delta;
    int gesturePointer1Delta;
    std::vector<InputListener*> listeners;
};

InputListener::InputListener(void) {
    InputManager::getInstance()->registerListener(this);
};

InputListener::~InputListener(void) {
    InputManager::getInstance()->unregisterListener(this);
};

#endif // __INPUTMANAGER_H__