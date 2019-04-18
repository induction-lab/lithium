#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__

#include "SpriteBatch.h"

class Activity: public ActivityHandler {
public:
	Activity(): isQuit(false) {}
protected:
	status onFirstStart() {
		Sprite* sprite = spriteBatch.registerSprite("textures/Play.png", 104, 100);
		sprite->setLocation(180, 190);		
		return STATUS_OK;
	}
	status onActivate() {
		return STATUS_OK;
	}
	void onDeactivate() {
		return;
	}
	status onStep() {
		if (isQuit) return STATUS_EXIT;
		return STATUS_OK;		
	}
private:
	SpriteBatch spriteBatch;
	bool isQuit;
 };

#endif
