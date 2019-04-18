#ifndef __TWEENCALLBACK_H__
#define __TWEENCALLBACK_H__

#include <functional>

class TweenCallback {
private:
    //void (*callbackFunc)(void*);
    //void* pointer;
    std::function<void()> callbackFunc;
public:
    TweenCallback(std::function<void()> callback = NULL):
        callbackFunc(callback) {
        //
    };
    ~TweenCallback() {};
    void set(std::function<void()> callback) {
        callbackFunc = callback;
    };
    void clear() {
        callbackFunc = NULL;
    };
    void call() {
        if (callbackFunc != NULL) {
            callbackFunc();
        }
    };
};

#endif // __TWEENCALLBACK_H__