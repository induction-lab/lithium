#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "EventLoop.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "GraphicsManager.h"
#include "Interface.h"

void Test() {};

class Engine: public ActivityHandler {
public:
	Engine(Context* context) :
		mTimeManager(context->mTimeManager),
		mGraphicsManager(context->mGraphicsManager),
		mInputManager(context->mInputManager),		
		mSoundManager(context->mSoundManager),
		mFirstStart(true),
		mQuit(false)
	{
		LOG_INFO("Creating Engine");
		mInterface = new Interface();
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
	}		
	void onMusicCheckBoxClick() {
		if (mMuteCheckBox->getChecked()) {
			mSoundManager->playMusic("sounds/Intro.mp3");
		} else {
			mSoundManager->stopMusic();
		}
	}
	void onExitButtonDown() {
		mSoundManager->playSound(mSoundDown);
	}
	void onExitButtonUp() {
		mSoundManager->playSound(mSoundUp);
	}
	void onExitButtonClick() {
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
			mMuteCheckBox = mInterface->addCheckBox("textures/Mute.png", 80, 78, Location(280, 185));
			mMuteCheckBox->setDownFunction(std::bind(&Engine::onMusicCheckBoxDown, this));
			mMuteCheckBox->setUpFunction(std::bind(&Engine::onMusicCheckBoxUp, this));
			mMuteCheckBox->setClickFunction(std::bind(&Engine::onMusicCheckBoxClick, this));
			mMuteCheckBox->setChecked(true);
			mExitButton = mInterface->addButton("textures/Exit.png", 80, 78, Location(80, 195));
			mExitButton->setDownFunction(std::bind(&Engine::onExitButtonDown, this));
			mExitButton->setUpFunction(std::bind(&Engine::onExitButtonUp, this));
			mExitButton->setClickFunction(std::bind(&Engine::onExitButtonClick, this));
			mPlayDragBox = mInterface->addDragBox("textures/Play.png", 104, 100, Location(180, 190));
			mFPSSpriteText = mInterface->addSpriteText("textures/Font.png", 32, 32, Location(20, 20), LEFT);
			mLogo = mInterface->addLogo("textures/Logo.png", 320, 170, Location(174, 480));
			mFirstStart = false;
		}
		mGraphicsManager->loadResources();
		mSoundManager->loadResources();
		if (mMuteCheckBox->getChecked()) mSoundManager->playMusic("sounds/Intro.mp3");
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
	void onSaveInstanceState(void** data, size_t* size) {
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
	DragBox* mPlayDragBox;
	SpriteText* mFPSSpriteText;
	Logo* mLogo;
	bool mFirstStart;
	bool mQuit;
 };

#endif
