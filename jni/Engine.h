#ifndef __LITHIUM_H__
#define __LITHIUM_H__

#include "EventLoop.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "GraphicsManager.h"
#include "Interface.h"

void Test() {};

class Engine: public ActivityHandler {
public:
	Engine(Context* pContext) :
			mTimeManager(pContext->mTimeManager),
			mGraphicsManager(pContext->mGraphicsManager),
			mInputManager(pContext->mInputManager),		
			mSoundManager(pContext->mSoundManager) {
		LOG_INFO("Creating Engine");
		mInterface = new Interface(mTimeManager, mGraphicsManager, mInputManager);
	}
	~Engine() {
		if (mInterface != NULL) delete mInterface;
		mInterface = NULL;		
	}
	void onMusicCheckBoxDown() {	
		mSoundManager->playSound(mSoundDown);
	}
	void onMusicCheckBoxUp() {
		mSoundManager->playSound(mSoundUp);
		mMute = !mMute;
		if (mMute == true) {
			mSoundManager->stopMusic();
		} else {
			mSoundManager->playMusic("sounds/Intro.mp3");
		}
	}
	void onExitButtonDown() {
		mSoundManager->playSound(mSoundDown);
	}
	void onExitButtonUp() {
		mSoundManager->playSound(mSoundUp);
		mQuit = true;
	}
protected:
	status onActivate() {
		// Starts services.
		if (mGraphicsManager->start() != STATUS_OK) return STATUS_ERROR;
		if (mInputManager->start() != STATUS_OK) return STATUS_ERROR;
		if (mSoundManager->start() != STATUS_OK) return STATUS_ERROR;
		mTimeManager->start();
		LOG_INFO("Activating Engine");
		if (mFirstStart) {
			mSoundDown = mSoundManager->registerSound("sounds/SoundDown.wav");
			mSoundUp = mSoundManager->registerSound("sounds/SoundUp.wav");			
			float x = mGraphicsManager->getRenderWidth()  * 0.5f;
			float y = mGraphicsManager->getRenderHeight() * 0.5f;
			mBackground = mInterface->addBackground("textures/Background.png", 360, 640, Location(x, y));
			mMuteCheckBox = mInterface->addCheckBox("textures/Mute.png", 80, 78, Location(280, 240));
			mMuteCheckBox->setDownFunction(std::bind(&Engine::onMusicCheckBoxDown, this));
			mMuteCheckBox->setUpFunction(std::bind(&Engine::onMusicCheckBoxUp, this));
			mMuteCheckBox->setChecked(true);
			mExitButton = mInterface->addButton("textures/Exit.png", 80, 78, Location(80, 240));
			mExitButton->setDownFunction(std::bind(&Engine::onExitButtonDown, this));
			mExitButton->setUpFunction(std::bind(&Engine::onExitButtonUp, this));
			mPlayButton = mInterface->addButton("textures/Play.png", 104, 100, Location(180, 240));
			mFPSSpriteText = mInterface->addSpriteText("textures/Font.png", 32, 32, Location(20, 20), Justification::LEFT);
			mLogo = mInterface->addLogo("textures/Logo.png", 320, 170, Location(174, 400));
			mFirstStart = false;
		}
		mGraphicsManager->loadResources();
		mSoundManager->loadResources();
		if (!mMute) mSoundManager->playMusic("sounds/Intro.mp3");
		return STATUS_OK;
	}
	void onDeactivate() {
		LOG_INFO("Deactivating Engine");
		mGraphicsManager->stop();
		mInputManager->stop();
		mSoundManager->stop();
		mTimeManager->stop();
	}
	status onStep() {
		// Updates services.
		mFPSSpriteText->setText(mTimeManager->getFrameRateStr());
		if (mGraphicsManager->update() != STATUS_OK) return STATUS_ERROR;
		if (mInputManager->update() != STATUS_OK) return STATUS_ERROR;
		mTimeManager->update();		
		mInterface->update();
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
	
	Interface*       mInterface;
	
	Sound* mSoundUp;
	Sound* mSoundDown;	
	
	Background* mBackground;
	Button* mExitButton;
	CheckBox* mMuteCheckBox;	
	Button* mPlayButton;
	SpriteText* mFPSSpriteText;
	Logo* mLogo;
	
	bool mFirstStart = true;
	bool mQuit = false;
	bool mMute = false;
 };

#endif
