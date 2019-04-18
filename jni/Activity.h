#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__

class Activity: public ActivityHandler {
private:	
	bool isQuitFlag;
	// Indicates if application is launched for the first time.
	bool isFirstStartFlag;
public:
	Activity():
		isQuitFlag(false),
		isFirstStartFlag(true) {
		LOG_DEBUG("Creating Activity.");
	}
	~Activity() {
		LOG_DEBUG("Destructing Activity.");
	}
protected:
	status onActivate() {
		LOG_DEBUG("Activate Engine.");
		if (isFirstStartFlag) {
			LOG_DEBUG("First start detected.");
			isFirstStartFlag = false;
		}
		return STATUS_OK;
	}
	status onStep() {
		if (isQuitFlag) return STATUS_EXIT;
		return STATUS_OK;		
	}
	void onDeactivate() {
		LOG_DEBUG("Dectivating Engine.");
	}
};


#endif
