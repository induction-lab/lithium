#ifndef __TIMEMANAGER_H__
#define __TIMEMANAGER_H__

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

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
float PlatformGetTime() {
	static bool	Initialized = false;
	static clock_t baseTime;
	if (Initialized == false) {
		baseTime = clock();
		Initialized = true;
		return 0.0f;
	}
	float appTime = (float)(clock() - baseTime) / (float)CLOCKS_PER_SEC;
	return appTime;
}

class TimeManager {
public:
	TimeManager() {
		LOG_INFO("Creating TimeManager");
		mLastTime = PlatformGetTime();
        mFrameRate[0] = '\0';
        mFPS = 0.0f;
        mFrameRateTime = mLastTime;
        mFrame = 0;
        reset();
	}
	~TimeManager() {
		LOG_INFO("Destructing TimeManager");
	}
    float start() {
		LOG_INFO("Starting TimeManager");
        mRunning = true;
        mStartTime = PlatformGetTime();
        return mTotalTimeBeforeLastStart;
    };
    float stop() {
		LOG_INFO("Stopping TimeManager");
        mRunning = false;
        float timeSinceLastStart = PlatformGetTime() - mStartTime;
        mTotalTimeBeforeLastStart += timeSinceLastStart;
        return mTotalTimeBeforeLastStart;
    }	
	void reset() {
		LOG_INFO("Resetting TimeManager");
        mRunning = false;
        mTotalTimeBeforeLastStart = 0.0f;
        mStartTime = 0.0f;
        mElapsedTime = 0.0f;
	}
    float getTime() {
        if(mRunning) {
            float timeSinceLastStart = PlatformGetTime() - mStartTime;
            return mTotalTimeBeforeLastStart + timeSinceLastStart;
        }
        return mTotalTimeBeforeLastStart;
    }
    float getFrameTime() {
        return mLastTime;
    }
    float getFrameElapsedTime() {
        return mElapsedTime;
    }	
	void update() {
        mFrame++;
        float currentTime = PlatformGetTime();
        mElapsedTime = currentTime - mLastTime;
        mLastTime = currentTime;
	}
    float getFrameRate()  {
        float currentTime = PlatformGetTime();
        if(currentTime - mFrameRateTime > 1.0f) {
            mFPS = mFrame / (currentTime - mFrameRateTime);
            mFrameRateTime = currentTime;
            mFrame = 0;
            return mFPS;
        }
        return mFPS;
    }
    const char* getFrameRateStr() {
        sprintf(mFrameRate, "%3.1f fps", getFrameRate());
        return mFrameRate;
    }
private:
    bool    mRunning;
    float 	mTotalTimeBeforeLastStart;
    float 	mStartTime;
    float 	mElapsedTime;
    float 	mLastTime;
    char    mFrameRate[16];
    float 	mFPS;
    float 	mFrameRateTime;
    unsigned int mFrame;
};

#endif
