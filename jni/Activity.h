#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__

#include "SpriteBatch.h"

class Activity: public ActivityHandler {
private:	
	Scene* scene;
	bool isQuitFlag;
	// Indicates if application is launched for the first time.
	bool isFirstStartFlag;
public:
	Activity():
		scene(NULL),
		isQuitFlag(false),
		isFirstStartFlag(true) {
		LOG_DEBUG("Creating Activity.");
	}
	~Activity() {
		LOG_DEBUG("Destructing Activity.");
		if (scene != NULL) SAFE_DELETE(scene);
	}
	void setStartScene();
	void setScene(Scene* nextScene);
protected:
	status onActivate() {
		LOG_DEBUG("Activate Engine.");
		if (isFirstStartFlag) {
			LOG_DEBUG("First start detected.");
			setStartScene();
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

#include "scenes\SecondScene.h"
#include "scenes\MainScene.h"

void Activity::setStartScene() {
	LOG_INFO("Set Start Scene");
	setScene(new MainScene(this));
};

void Activity::setScene(Scene* nextScene) {
	LOG_INFO("Set Scene");
	if (scene != NULL) SAFE_DELETE(scene);
	scene = nextScene;
	scene->start();
};

#endif
