#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

class Temp: public InputListener {
public:
	void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
		if (event == Touch::TOUCH_RELEASE) {
			call();
		}
	}
	void set(void(*callback)()) {
		callbackFunc = callback;
	};
	void call() {
		if (callbackFunc != NULL) {
			callbackFunc();
		}
	};	
	void (*callbackFunc)();	
};

class MainScene;
MainScene* mainScene;

class MainScene: public Scene {
public:
	Temp temp;
	MainScene(Activity* activity):
		Scene(activity) {	
		LOG_INFO("MainScene create.");
		mainScene = this;
		temp.set(&call);
		//Texture* texture = GraphicsManager::getInstance()->loadTexture("textures/Play.png", GL_NEAREST, GL_CLAMP_TO_EDGE);
	}
	~MainScene() {
		LOG_INFO("MainScene delete.");
	}
	static void call(void) {
		LOG_INFO("GO!");
		mainScene->call2();
	}
	void call2(void) {
		activity->setScene(new SecondScene(activity));
	}	
};


#endif
