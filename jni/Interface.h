#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <functional>
#include "SoundManager.h"
#include "GraphicsManager.h"
#include "Sprite.h"

class Interface;

class CheckBox {
public:
	CheckBox(SoundManager* pSoundManager, Sprite* pSprite, const char* pName, Location pLocation, std::function<void()> pClickHandler):
		mSoundManager(pSoundManager),
		mSprite(pSprite),
		mName(pName),
		mPressed(false),
		mChecked(false),
		mClickHandler(pClickHandler) {
		mSprite->setLocation(pLocation);
		mSoundUp = mSoundManager->registerSound("sounds/Interface_Button_Up.wav");
		mSoundDown = mSoundManager->registerSound("sounds/Interface_Button_Down.wav");
	}
	void setPressed(bool pPressed) { 
		if (mPressed == pPressed) return;
		mPressed = pPressed;
		if (mPressed && !mChecked)  {
			mSprite->setAnimation(1, 1, 0.0f, false);
			mSoundManager->playSound(mSoundDown);			
			mClickHandler();
			mChecked = !mChecked;
			return;
		}
		if (mPressed && mChecked)  {
			mSprite->setAnimation(0, 1, 0.0f, false);
			mSoundManager->playSound(mSoundUp);			
			mClickHandler();			
			mChecked = !mChecked;
			return;			
		}		
	}
	void setCheck(bool pChecked) {
		//
	}
	//void OnClick(const std::function<void()>& callback) { mClickHandler = callback; }
private:
	friend class Interface;
	SoundManager* mSoundManager;
	Sound* mSoundUp;
	Sound* mSoundDown;
	Sprite* mSprite;
	const char* mName;
	bool mPressed;	
	bool mChecked;
	std::function<void()> mClickHandler;
};

class Button {
public:
	Button(SoundManager* pSoundManager, Sprite* pSprite, const char* pName, Location pLocation, std::function<void()> pClickHandler):
		mSoundManager(pSoundManager),
		mSprite(pSprite),
		mName(pName),
		mPressed(false),
		mClickHandler(pClickHandler) {
		mSprite->setLocation(pLocation);
		mSoundUp = mSoundManager->registerSound("sounds/Interface_Button_Up.wav");
		mSoundDown = mSoundManager->registerSound("sounds/Interface_Button_Down.wav");
	}
	void setPressed(bool pPressed) { 
		if (mPressed == pPressed) return;
		mPressed = pPressed;
		if (mPressed) {
			mSprite->setAnimation(1, 1, 0.0f, false);
			mSoundManager->playSound(mSoundDown);			
		} else {
			mSprite->setAnimation(0, 1, 0.0f, false);
			mSoundManager->playSound(mSoundUp);
			mClickHandler();
		}		
	}
	//void OnClick(const std::function<void()>& callback) { mClickHandler = callback; }
private:
	friend class Interface;
	SoundManager* mSoundManager;
	Sound* mSoundUp;
	Sound* mSoundDown;
	Sprite* mSprite;
	const char* mName;
	bool mPressed;
	std::function<void()> mClickHandler;
};

class Interface {
public:
	Interface(SpriteBatch* pSpriteBatch, InputManager* pInputManager, SoundManager* pSoundManager) :
	mSpriteBatch(pSpriteBatch),
	mInputManager(pInputManager),
	mSoundManager(pSoundManager) {
		//
	}
	void addButton(const char* pName, const char* pPath, Location pLocation, std::function<void()> pClickHandler) {
		LOG_INFO("Adds Button widget with name %s", pName);
		Sprite* sprite = mSpriteBatch->registerSprite(pPath, 128, 128);
		Button* button = new Button(mSoundManager, sprite, pName, pLocation, pClickHandler);
		mButtons.push_back(button);
	}
	void addCheckBox(const char* pName, const char* pPath, Location pLocation, std::function<void()> pClickHandler) {
		LOG_INFO("Adds CheckBox widget with name %s", pName);
		Sprite* sprite = mSpriteBatch->registerSprite(pPath, 128, 128);
		CheckBox* checkBox = new CheckBox(mSoundManager, sprite, pName, pLocation, pClickHandler);
		mCheckBoxes.push_back(checkBox);
	}	
	bool pointInSprite(Sprite* pSprite, Location pLocation) {
		Location location = pSprite->getLocation();
		float halfWidth = (float)pSprite->getSpriteWidth() * 0.5f;
		float halfHeight = (float)pSprite->getSpriteHeight() * 0.5f;
		return ((pLocation.x > location.x - halfWidth ) &&
				(pLocation.x < location.x + halfWidth ) &&
				(pLocation.y > location.y - halfHeight) &&
				(pLocation.y < location.y + halfHeight));
	}
	void update() {
		Location location(mInputManager->getDownX(), mInputManager->getDownY());
		std::vector<Button*>::iterator buttonIt;
		for (buttonIt = mButtons.begin(); buttonIt < mButtons.end(); ++buttonIt) {
			(*buttonIt)->setPressed(pointInSprite((*buttonIt)->mSprite, location));
        }
		std::vector<CheckBox*>::iterator checkBoxIt;
		for (checkBoxIt = mCheckBoxes.begin(); checkBoxIt < mCheckBoxes.end(); ++checkBoxIt) {
			(*checkBoxIt)->setPressed(pointInSprite((*checkBoxIt)->mSprite, location));
        }		
	}
private:
	SpriteBatch* mSpriteBatch;
	InputManager* mInputManager;
	SoundManager* mSoundManager;
	std::vector<Button*> mButtons;
	std::vector<CheckBox*> mCheckBoxes;
};

#endif