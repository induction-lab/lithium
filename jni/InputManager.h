#ifndef __INPUTMANAGER_H__
#define __INPUTMANAGER_H__

#include <math.h>
#include <android_native_app_glue.h>
#include <android/sensor.h>
#include <vector>

#include "Singleton.h"

// #define INPUTMANAGER_LOG_EVENTS

struct Location {
    Location(): x(0.0f), y(0.0f) {};
    Location(float X, float Y): x(X), y(Y) {};
    float x;
    float y;
};

// Defines a touch screen event.
class Touch {
public:
    static const unsigned int MAX_TOUCH_POINTS = 10; // Maximum simultaneous touch points supported.
    enum TouchEvent {
        TOUCH_PRESS,
        TOUCH_RELEASE,
        TOUCH_MOVE
    };
private:
    Touch() {};
};

static const bool MULTI_TOUTCH = true;

class InputListener {
public:
    InputListener(void);
    virtual ~InputListener(void);
    virtual void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {};
};

class InputManager: public Singleton<InputManager> {
private:
    void onTouchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        for (std::vector<InputListener*>::iterator it = listeners.begin(); it < listeners.end(); ++it) {
            (*it)->touchEvent(event, x, y, pointerId);
        }
    };
    int primaryTouchId;
    std::vector<InputListener*> listeners;
public:
    InputManager():
        primaryTouchId(-1) {
        LOG_INFO("Creating InputManager.");
    }
    ~InputManager() {
        LOG_INFO("Destructing InputManager.");
    }
    status start() {
        LOG_INFO("Starting InputManager.");
        return STATUS_OK;
    }
    status update() {
        // Clears previous state.
        return STATUS_OK;
    };
    void stop() {
        LOG_INFO("Stopping InputManager.");
    }
public:
    void registerListener(InputListener *listener) {
        listeners.push_back(listener);
    }
    void unregisterListener(InputListener *listener) {
        listeners.erase(std::find(listeners.begin(), listeners.end(), listener));
    }
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
#endif
        int32_t action = AMotionEvent_getAction(event);
        size_t pointerIndex;
        size_t pointerId;
        size_t pointerCount;
        int x, y;
        switch (action & AMOTION_EVENT_ACTION_MASK) {
        case AMOTION_EVENT_ACTION_DOWN:
            pointerId = AMotionEvent_getPointerId(event, 0);
            x = AMotionEvent_getX(event, 0);
            y = AMotionEvent_getY(event, 0);
            // Primary pointer down.
            onTouchEvent(Touch::TOUCH_PRESS, x, y, pointerId);
            primaryTouchId = pointerId;
            break;
        case AMOTION_EVENT_ACTION_UP:
            pointerId = AMotionEvent_getPointerId(event, 0);
            x = AMotionEvent_getX(event, 0);
            y = AMotionEvent_getY(event, 0);
            if (MULTI_TOUTCH || primaryTouchId == pointerId) {
                onTouchEvent(Touch::TOUCH_RELEASE, x, y, pointerId);
            }
            primaryTouchId = -1;
            break;
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            pointerId = AMotionEvent_getPointerId(event, pointerIndex);
            x = AMotionEvent_getX(event, pointerIndex);
            y = AMotionEvent_getY(event, pointerIndex);
            // Non-primary pointer down.
            if (MULTI_TOUTCH) {
                onTouchEvent(Touch::TOUCH_PRESS, AMotionEvent_getX(event, pointerIndex), AMotionEvent_getY(event, pointerIndex), pointerId);
            }
            break;
        case AMOTION_EVENT_ACTION_POINTER_UP:
            pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            pointerId = AMotionEvent_getPointerId(event, pointerIndex);
            x = AMotionEvent_getX(event, pointerIndex);
            y = AMotionEvent_getY(event, pointerIndex);
            if (MULTI_TOUTCH || primaryTouchId == pointerId) {
                onTouchEvent(Touch::TOUCH_RELEASE, x, y, pointerId);
            }
            if (primaryTouchId == pointerId) primaryTouchId = -1;
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            // ACTION_MOVE events are batched, unlike the other events.
            pointerCount = AMotionEvent_getPointerCount(event);
            for (size_t i = 0; i < pointerCount; ++i) {
                pointerId = AMotionEvent_getPointerId(event, i);
                x = AMotionEvent_getX(event, i);
                y = AMotionEvent_getY(event, i);
                if (MULTI_TOUTCH || primaryTouchId == pointerId) {
                    onTouchEvent(Touch::TOUCH_MOVE, x, y, pointerId);
                }
            }
            break;
        }
        return 0;
    }
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
#endif
        return 0;
    }
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
#endif
        return 0;
    }
    int32_t onAccelerometerEvent(ASensorEvent* event) {
#ifdef INPUTMANAGER_LOG_EVENTS
        LOG_DEBUG("ASensorEvent=%d", event->version);
        LOG_DEBUG("ASensorEvent=%d", event->sensor);
        LOG_DEBUG("ASensorEvent=%d", event->timestamp);
        LOG_DEBUG("ASensorEvent=%d", event->type);
        LOG_DEBUG("ASensorEvent=%f,%f,%f,%d", event->acceleration.x, event->acceleration.y, event->acceleration.z, event->acceleration.status);
#endif
        return 0;
    }
};

InputListener::InputListener(void) {
    InputManager::getInstance()->registerListener(this);
}

InputListener::~InputListener(void) {
    InputManager::getInstance()->unregisterListener(this);
}

#endif
