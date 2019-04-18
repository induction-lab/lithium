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
double PlatformGetTime() {
	// Retrieves current time with a monotonic clock to ensure
	// time goes forward and is not subject to system changes.
	timespec timeVal;
	clock_gettime(CLOCK_MONOTONIC, &timeVal);
	return timeVal.tv_sec + (timeVal.tv_nsec * 1.0e-9);
}

class TimeManager: public Singleton<TimeManager> {
private:
	bool running;
	float elapsedTime;
	double lastTime;
	char frameRate[20];
	float fps;
	float frameRateTime;
	unsigned int frame;
public:
	TimeManager() {
		LOG_DEBUG("Creating TimeManager.");
		lastTime = PlatformGetTime();
		frameRate[0] = '\0';
		fps = 0.0f;
		frameRateTime = lastTime;
		frame = 0;
	}
	~TimeManager() {
		LOG_DEBUG("Destructing TimeManager.");
	}
	float getLastTime() {
		return lastTime;
	}
	float getElapsedTime() {
		return elapsedTime;
	}
	void update() {
		frame++;
		double currentTime = PlatformGetTime();
		elapsedTime = currentTime - lastTime;
		lastTime = currentTime;
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
};

#endif
