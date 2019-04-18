#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

class Gameplay : public Scene {
private:
    Activity* activity;
public:
	Gameplay(Activity* activity):
		activity(activity) {
		LOG_INFO("Scene Gameplay created.");
	}
	~Gameplay() {
		LOG_INFO("Scene Gameplay Destructed.");
	}
    status start() {
        LOG_DEBUG("Start Gameplay scene.");
        return STATUS_OK;
    }
    void update() {
        //
    }
    void touchRelease(Location location) {
        activity->setStartScene();
    }    
};

#endif
