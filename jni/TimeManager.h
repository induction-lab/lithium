#ifndef __TIMEMANAGER_H__
#define __TIMEMANAGER_H__

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "Singleton.h"

// Build date.
char BuildDate[64];
const char* GetBuildDate() {
    int month, day, year;
    const char* buff[255];
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    sscanf(__DATE__, "%s %d %d", BuildDate, &day, &year);
    month = (strstr(month_names, BuildDate)-month_names) / 3 + 1;
    sprintf(BuildDate, "%02d.%02d.%d", day, month, year);
    return BuildDate;
}

// Platform-dependent function to get the current time (in seconds).
double PlatformGetTime() {
    // Retrieves current time with a monotonic clock to ensure
    // time goes forward and is not subject to system changes.
    timespec timeVal;
    clock_gettime(CLOCK_MONOTONIC, &timeVal);
    return timeVal.tv_sec + (timeVal.tv_nsec * 1.0e-9);
}

class TimeManager: public Singleton<TimeManager> {
public:
    TimeManager() {
        LOG_INFO("Creating TimeManager.");
        lastTime = PlatformGetTime();
        frameRate[0] = '\0';
        fps = 0.0f;
        frameRateTime = lastTime;
        frame = 0;
        reset();
    }
    ~TimeManager() {
        LOG_INFO("Destructing TimeManager.");
    }
    status start() {
        LOG_INFO("Starting TimeManager.");
        running = true;
        startTime = PlatformGetTime();
        return STATUS_OK;
    };
    void stop() {
        LOG_INFO("Stopping TimeManager.");
        running = false;
        float timeSinceLastStart = PlatformGetTime() - startTime;
        totalTimeBeforeLastStart += timeSinceLastStart;
    }
    void reset() {
        LOG_INFO("Resetting TimeManager.");
        running = false;
        totalTimeBeforeLastStart = 0.0f;
        startTime = 0.0f;
        elapsedTime = 0.0f;
    }
    float getTime() {
        if(running) {
            float timeSinceLastStart = PlatformGetTime() - startTime;
            return totalTimeBeforeLastStart + timeSinceLastStart;
        }
        return totalTimeBeforeLastStart;
    }
    float getFrameTime() {
        return lastTime;
    }
    float getFrameElapsedTime() {
        return elapsedTime;
    }
    status update() {
        frame++;
        double currentTime = PlatformGetTime();
        elapsedTime = currentTime - lastTime;
        lastTime = currentTime;
        return STATUS_OK;
    }
    float getFrameRate()  {
        float currentTime = PlatformGetTime();
        if(currentTime - frameRateTime > 1.0f) {
            fps = frame / (currentTime - frameRateTime);
            frameRateTime = currentTime;
            frame = 0;
            return fps;
        }
        return fps;
    }
    const char* getFrameRateStr() {
        sprintf(frameRate, "%3.0f fps\n", getFrameRate());
        return frameRate;
    }
private:
    bool running;
    float elapsedTime;
    float totalTimeBeforeLastStart;
    double startTime;
    double lastTime;
    char frameRate[20];
    float fps;
    float frameRateTime;
    unsigned int frame;
};

#endif
