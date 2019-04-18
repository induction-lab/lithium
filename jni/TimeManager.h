#ifndef __TIMEMANAGER_H__
#define __TIMEMANAGER_H__

#include <time.h>
#include <stdlib.h>

class TimeManager {
public:
    TimeManager() :
            mElapsed(0.0f),
            mLastTime(0.0f) {
        LOG_INFO("Creating TimeManager");
        srand(time(NULL));
    }
    ~TimeManager() {
        LOG_INFO("Destructing TimeManager");
    }
    void reset() {
        LOG_INFO("Resetting TimeManager");
        mElapsed = 0.0f;
        mLastTime = now();
    }
    void update() {
        // Checks elapsed time since last frame. It is important to
        // work on double with current time to avoid losing accuracy
        // Then we can go back to float for elapsed time.
        double lCurrentTime = now();
        mElapsed = (lCurrentTime - mLastTime);
        mLastTime = lCurrentTime;
    }
    double now() {
        // Retrieves current time with a monotonic clock to ensure
        // time goes forward and is not subject to system changes.
        timespec lTimeVal;
        clock_gettime(CLOCK_MONOTONIC, &lTimeVal);
        return lTimeVal.tv_sec + (lTimeVal.tv_nsec * 1.0e-9);
    }
    float elapsed() {
        return mElapsed;
    }
private:
    float mElapsed;
    double mLastTime;
};

#endif
