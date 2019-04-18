#ifndef __SCENE_H__
#define __SCENE_H__

class Activity;

class Scene {
public:
	Scene(Activity* activity):
		activity(activity) {
		LOG_INFO("Scene create.");
	};
	virtual ~Scene() {
		LOG_INFO("Scene delete");
	}
	virtual status start() = 0;	
	Activity* activity;
};

#endif

