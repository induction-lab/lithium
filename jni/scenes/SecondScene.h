#ifndef __SECOND_SCENE_H__
#define __SECOND_SCENE_H__

class SecondScene: public Scene {
public:	
	SecondScene(Activity* activity):
		Scene(activity) {
		LOG_INFO("SecondScene create");
	}
	~SecondScene() {
		LOG_INFO("SecondScene delete.");
	}	
};

#endif
