#ifndef __TWEENCALLBACK_H__
#define __TWEENCALLBACK_H__

#include <functional>

class Tweenable {
public:
    virtual int getValues(int tweenType, float* returnValues) = 0;
    virtual void setValues(int tweenType, float* newValues) = 0;
};

class TweenCallback {
private:
    // Depricated ...
    // void (*callbackFunc)(void*);
    // void* pointer;
    std::function<void(Tweenable*)> callbackFunc;
public:
    TweenCallback(std::function<void(Tweenable*)> callback = NULL):
        callbackFunc(callback) {
        //
    };
    ~TweenCallback() {};
    void set(std::function<void(Tweenable*)> callback) {
        callbackFunc = callback;
    };
    void clear() {
        callbackFunc = NULL;
    };
    void call(Tweenable* t) {
        if (callbackFunc != NULL && t != NULL) callbackFunc(t);
    };
};

#endif // __TWEENCALLBACK_H__