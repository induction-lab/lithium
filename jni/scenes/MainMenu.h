#ifndef __MAINMENU_H__
#define __MAINMENU_H__

class MainMenu : public Scene {
private:
    Activity* activity;
public:
	MainMenu(Activity* activity):
		activity(activity) {
		LOG_INFO("Scene MainMenu created.");
	};
	~MainMenu() {
		LOG_INFO("Scene MainMenu Destructed.");
	}
    status start() {
        LOG_INFO("Start MainMenu scene.");
        float renderWidth = (float) GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float) GraphicsManager::getInstance()->getRenderHeight();
        SpriteBatch *spriteBatch = new SpriteBatch();
        
        Sprite* background = spriteBatch->registerSprite("textures/Background.png", 360, 640);
        background->setLocation(renderWidth / 2, renderHeight / 2);
        
        Sprite* gameBox = spriteBatch->registerSprite("textures/GameBox.png", 360, 380);
        gameBox->setLocation(renderWidth / 2, renderHeight / 2);
        
        sprite = spriteBatch->registerSprite("textures/Play.png", 104, 100);
        sprite->setLocation(renderWidth / 2, renderHeight / 2);
        Tween* t0 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_X, 1.0f, Ease::Sinusoidal::InOut)
            ->target(1.2f)->remove(false)->loop()->reverse()->start();
        Tween* t1 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_Y, 1.0f, Ease::Sinusoidal::InOut)
            ->target(1.2f)->remove(false)->loop()->reverse()->start(0.5f);
        return STATUS_OK;
    }
    void update() {
        //
    }
    void touchRelease(Location location) {
        if (sprite->pointInSprite(location)) {
			LOG_INFO("Tesy passed!");
            activity->ChangeScene(new Gameplay(activity));
		} else LOG_INFO("TEST");
    }
    Sprite* sprite;
};

#endif // __MAIN_MENU_H__
