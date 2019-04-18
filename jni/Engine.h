#ifndef __LITHIUM_H__
#define __LITHIUM_H__

#include "EventLoop.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "GraphicsManager.h"
#include "Sprite.h"
#include "Interface.h"

class Engine: public ActivityHandler {
public:
	Engine(Context* pContext) :
			mGraphicsManager(pContext->mGraphicsManager),
			mInputManager(pContext->mInputManager),
			mSoundManager(pContext->mSoundManager),
			mTimeManager(pContext->mTimeManager),
			mSpriteBatch(mTimeManager, mGraphicsManager),
			mInterface(&mSpriteBatch, mInputManager, mSoundManager)
	{
		LOG_INFO("Creating Engine");
		Labels = mSpriteBatch.registerSprite("textures/Labels.png", 338, 600);
	}
	~Engine() {}
	void Test1() {
		mMute = !mMute;
		if (mMute == true) {
			mSoundManager->stopMusic();
		} else {
			mSoundManager->playMusic("sounds/background.mp3");
		}
	}	
	void Test2() {
		mQuit = true;
	}		
protected:
	status onActivate() {
		// Starts services.
		if (mGraphicsManager->start() != STATUS_OK) return STATUS_ERROR;
		if (mInputManager->start() != STATUS_OK) return STATUS_ERROR;
		if (mSoundManager->start() != STATUS_OK) return STATUS_ERROR;
		mTimeManager->reset();
		LOG_INFO("Activating Engine");
		if (mFirstStart) {
			float x = mGraphicsManager->getRenderWidth()  * 0.5f;
			float y = mGraphicsManager->getRenderHeight() * 0.5f;
			mInterface.addCheckBox("Music", "textures/Interface_Button_Aqua.png", Location(x - 100, y), std::bind(&Engine::Test1, this));
			mInterface.addButton("Exit", "textures/Interface_Button_Red.png", Location(x + 100, y), std::bind(&Engine::Test2, this));
			Labels->setLocation(Location(x, y));
			mFirstStart = false;
		}
		if (mMute != true) mSoundManager->playMusic("sounds/background.mp3");
		return STATUS_OK;
	}
	void onDeactivate() {
		LOG_INFO("Deactivating Engine");
		mGraphicsManager->stop();
		mInputManager->stop();
		mSoundManager->stop();
	}
	status onStep() {
		// Updates services.
		if (mGraphicsManager->update() != STATUS_OK) return STATUS_ERROR;
		if (mInputManager->update() != STATUS_OK) return STATUS_ERROR;
		mTimeManager->update();		
		mInterface.update();
		if (mQuit) return STATUS_EXIT;
		return STATUS_OK;		
	}
	void onStart() {
		LOG_INFO("onStart");
	}
	void onResume() {
		LOG_INFO("onResume");
	}
	void onPause() {
		LOG_INFO("onPause");
	}
	void onStop() {
		LOG_INFO("onStop");
	}
	void onDestroy() {
		LOG_INFO("onDestroy");
	}
	void onSaveInstanceState(void** pData, size_t* pSize) {
		LOG_INFO("onSaveInstanceState");
	}
	void onConfigurationChanged() {
		LOG_INFO("onConfigurationChanged");
	}
	void onLowMemory() {
		LOG_INFO("onLowMemory");
	}
	void onCreateWindow() {
		LOG_INFO("onCreateWindow");
	}
	void onDestroyWindow() {
		LOG_INFO("onDestroyWindow");
	}
	void onGainFocus() {
		LOG_INFO("onGainFocus");
	}
	void onLostFocus() {
		LOG_INFO("onLostFocus");
	}
private:
	GraphicsManager* mGraphicsManager;
	InputManager*    mInputManager;
	SoundManager*    mSoundManager;
	TimeManager*     mTimeManager;
	SpriteBatch      mSpriteBatch;
	Interface        mInterface;
	
	Sprite* Labels;
	bool mFirstStart = true;
	bool mQuit = false;
	bool mMute = true;
 };

#endif
