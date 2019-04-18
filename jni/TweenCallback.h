#ifndef __TWEENCALLBACK_H__
#define __TWEENCALLBACK_H__

class TweenCallback {
private:
    void (*callbackFunc)(void*);
    void* pointer;
public:
    TweenCallback(void(*callback)(void*) = NULL, void* userPointer = NULL):
        callbackFunc(callback),
        pointer(userPointer) {
        //
    };
    ~TweenCallback() {};
    void set(void(*callback)(void*), void* userPointer) {
        callbackFunc = callback;
        pointer = userPointer;
    };
    void set(void(*callback)(void*)) {
        callbackFunc = callback;
    };
    void clear() {
        callbackFunc = NULL;
        pointer = NULL;
    };
    void call() {
        if (callbackFunc != NULL) {
            callbackFunc(pointer);
        }
    };
};

#endif