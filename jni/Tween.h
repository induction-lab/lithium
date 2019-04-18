#ifndef __TWEEN_H__
#define __TWEEN_H__

#include <list>

#include "TimeManager.h"
#include "TweenEasings.h"
#include "TweenCallback.h"

template<class T>
static T lerp(T a, T b, float k) {
    return a * (1.0f - k) + b * k;
};

class Tweenable {
public:
    virtual int getValues(int tweenType, float* returnValues) = 0;
    virtual void setValues(int tweenType, float* newValues) = 0;
};

class Tween {
private:
    static int combinedAttrsLimit;
public:
    Tween(Tweenable* targetObj, int tweenType, float duration = 1, EaseFunc ease = Ease::Linear):
        started(false),
        playing(false),
        complited(false),
        isReverseFlag(false),
        isAutoRemoveFlag(true),
        targetObj(targetObj),
        type(tweenType),
        duration(duration),
        easing(ease),
        delayAmount(0.0f),
        elapsed(0.0f),
        repeat(0) {
        startValues = new float[combinedAttrsLimit];
        targetValues = new float[combinedAttrsLimit];
        accessorBuffer = new float[combinedAttrsLimit];
        accessorBufferSize = combinedAttrsLimit;
    };
    ~Tween() {
        delete startValues;
        delete targetValues;
        delete accessorBuffer;
    };
    Tween* clear() {
        onStartCallback.clear();
        onUpdateCallback.clear();
        onCompleteCallback.clear();
        chains.clear();
        return this;
    };
    Tween* onStart(std::function<void()> onStartFunc) {
        onStartCallback.set(onStartFunc);
        return this;
    };
    Tween* onUpdate(std::function<void()> onUpdateFunc) {
        onUpdateCallback.set(onUpdateFunc);
        return this;
    };
    Tween* onComplete(std::function<void()> onCompleteFunc) {
        onCompleteCallback.set(onCompleteFunc);
        return this;
    };
    Tween* setEase(EaseFunc ease) {
        easing = ease;
        return this;
    };
    Tween* delay(float amount) {
        startTime -= delayAmount;
        delayAmount = amount;
        startTime += delayAmount;
        endTime = startTime + duration;
        return this;
    };
    Tween* start(float initialDelay = 0) {
        playing = true;
        complited = false;
        startTime = TimeManager::getInstance()->getTime() + delayAmount + initialDelay;
        endTime = startTime + duration;
        if (targetObj != NULL) combinedAttrsCnt = targetObj->getValues(type, startValues);
        return this;
    };
    Tween* loop(int count = -1) {
        repeat = count;
        return this;
    };
    Tween* reverse(bool value = true) {
        isReverseFlag = value;
        return this;
    };
    Tween* remove(bool value = true) {
        isAutoRemoveFlag = value;
        return this;
    };
    // Chaining.
    Tween* addChain(Tween* chainedTween) {
        chains.push_back(chainedTween);
        return this;
    };
    // Getters.
    bool getAutoRemove() {
        return isAutoRemoveFlag;
    };
    bool getCompleted() {
        return complited;
    };
    // Changes the limit for combined attributes. Defaults to 3 to reduce memory footprint.
    static void setCombinedAttributesLimit(int limit) {
        Tween::combinedAttrsLimit = limit;
    };
    // Sets the target value of the interpolation.
    Tween* target(float targetValue) {
        targetValues[0] = targetValue;
        return this;
    };
    // Sets the target values of the interpolation.
    Tween* target(float targetValue1, float targetValue2) {
        targetValues[0] = targetValue1;
        targetValues[1] = targetValue2;
        return this;
    };
    // Sets the target values of the interpolation.
    Tween* target(float targetValue1, float targetValue2, float targetValue3) {
        targetValues[0] = targetValue1;
        targetValues[1] = targetValue2;
        targetValues[2] = targetValue3;
        return this;
    };
    // Sets the target values of the interpolation.
    Tween* target(float *targetValues, int len) {
        if (len <= combinedAttrsLimit) for (int i = 0; i < len; i++) this->targetValues[i] = targetValues[i];
        return this;
    };
    void update(float t) 	{
        // If it's not ready return.
        if (!playing || t < startTime) return;
        // On start callback.
        if (!started) {
            started = true;
            complited = false;
            onStartCallback.call();
        }
        // Progress.
        elapsed = (t - startTime) / duration;
        // Update.
        if (!complited) {
            if (elapsed >= 1) {
                // Complete it.
                targetObj->setValues(type, targetValues);
                complited = true;
                // loop.
                if (repeat != 0) {
                    if (repeat > 0) repeat--;
                    started = complited = false;
                    startTime = endTime + delayAmount;
                    endTime = startTime + duration;
                }
                // Reverse.
                if (isReverseFlag) {
                    for (int i = 0; i < combinedAttrsCnt; i++) {
                        float tmp = startValues[i];
                        startValues[i] = targetValues[i];
                        targetValues[i] = tmp;
                    }
                };
                // Start any chains.
                for (std::list<Tween*>::reverse_iterator it = chains.rbegin(); it != chains.rend(); ++it) (*it)->start();
                // Complete callback.
                onCompleteCallback.call();
            } else {
                // Update it.
                for (int i = 0; i < combinedAttrsCnt; i++) accessorBuffer[i] = lerp(startValues[i], targetValues[i], easing(elapsed));
                targetObj->setValues(type, accessorBuffer);
                onUpdateCallback.call();
            }
        }
    };
protected:
    TweenCallback onStartCallback;
    TweenCallback onUpdateCallback;
    TweenCallback onCompleteCallback;
    std::list<Tween*> chains;
    float startTime, endTime, delayAmount, duration, elapsed;
    bool started, complited, isReverseFlag, playing, isAutoRemoveFlag;
    EaseFunc easing;
    int repeat;
    // Main.
    Tweenable *targetObj;
    int type;
    // Values.
    float* startValues;
    float* targetValues;
    // Buffers.
    float *accessorBuffer;
    int accessorBufferSize;
    int combinedAttrsCnt;
};

int Tween::combinedAttrsLimit = 3;

#endif // __TWEEN_H__