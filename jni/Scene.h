#ifndef __SCENE_H__
#define __SCENE_H__

#include "InputManager.h"
#include "GraphicsManager.h"
#include "SpriteBatch.h"

#include <functional>

class Widget: public InputListener {
public:
    Widget(int width, int height, Location location):
        width(width), height(height), location(location),
        sprite(NULL),
        downFunction(NULL),
        upFunction(NULL),
        clickFunction(NULL) {
        //
    };
    virtual void update() {};
    void setDownFunction(std::function<void()> callback) {
        downFunction = callback;
    };
    void setUpFunction(std::function<void()> callback) {
        upFunction = callback;
    };
    void setClickFunction(std::function<void()> callback) {
        clickFunction = callback;
    };
    void setSprite(Sprite* sprite) {
        this->sprite = sprite;
        sprite->setLocation(location.x, location.y);
    };
    Sprite* sprite;
protected:
    friend class Scene;
    int width, height;
    Location location;
    SpriteBatch* spriteBatch;
    std::function<void()> downFunction;
    std::function<void()> upFunction;
    std::function<void()> clickFunction;
};

class Button: public Widget {
public:
    Button(int width, int height, Location location): Widget(width, height, location) {
        //
    };
    void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        bool inSprite = sprite->pointInSprite(point.x, point.y);
        switch (event) {
        case Touch::TOUCH_DOWN:
            if (inSprite) {
                sprite->setFrames(1, 1, 0.0f, false);
                if (downFunction != NULL) downFunction();
            }
            break;
        case Touch::TOUCH_UP:
            sprite->setFrames(0, 1, 0.0f, false);
            if (inSprite) {
                if (upFunction != NULL) upFunction();
            }
            break;
        case Touch::TOUCH_MOVE:
            if (!inSprite) {       
                sprite->setFrames(0, 1, 0.0f, false);
            }
        }
    };
    int gestureTapEvent(int x, int y) {
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        if (sprite->pointInSprite(point.x, point.y)) {
            if (clickFunction != NULL) {
                clickFunction();
                return 1;
            }
        }
        return 0;
    };  
    int gestureLongTapEvent(int x, int y, float time) {
        return gestureTapEvent(x, y);
    };
};

class CheckBox: public Widget {
public:
    CheckBox(int width, int height, Location location): Widget(width, height, location),
        checked(false) {
        //
    };
    void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        bool inSprite = sprite->pointInSprite(point.x, point.y);
        switch (event) {
        case Touch::TOUCH_DOWN:
            if (inSprite) {
                if (downFunction != NULL) downFunction();               
            }
            break;
        case Touch::TOUCH_UP:
            if (inSprite) {
                if (upFunction != NULL) upFunction();
            }
            break;
        default:
            break;
        }
    };
    int gestureTapEvent(int x, int y) {
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        if (sprite->pointInSprite(point.x, point.y)) {
            checked = !checked;
            setChecked(checked);
            return 1;
        }
        return 0;
    };
    int gestureLongTapEvent(int x, int y, float time) {
        return gestureTapEvent(x, y);
    };
    bool getChecked() {
        return checked;
    };
    void setChecked(bool checked) {
        this->checked = checked;        
        if (checked) {
            sprite->setFrames(1, 1, 0.0f, false);
        } else {
            sprite->setFrames(0, 1, 0.0f, false);
        }
        if (clickFunction != NULL) clickFunction();
    };
private:
    bool checked;
};

class Slider: public Widget {
public:
    Slider(int width, int height, Location location): Widget(width, height, location) {};
    void setSliderHandle(const char* path, int width, int height, int position) {
        this->sliderHandle = spriteBatch->registerSprite(path, width, height);
        int x = location.x - sprite->getWidth() / 2;
        float minWidth = sprite->getLocation().x - sprite->getWidth() / 2 + sliderHandle->getWidth() / 4;
        sliderHandle->setLocation(minWidth, location.y);
    };
    int gestureDragEvent(int x, int y) {
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        if (sliderHandle->pointInSprite(point.x, point.y)) {
            float minWidth = sprite->getLocation().x - sprite->getWidth() / 2 + sliderHandle->getWidth() / 4;
            float maxWidth = sprite->getLocation().x + sprite->getWidth() / 2 - sliderHandle->getWidth() / 4;
            if (point.x < minWidth) {
                sliderHandle->setLocation(minWidth, location.y);
                return 0;
            }
            if (point.x > maxWidth) {
                sliderHandle->setLocation(maxWidth, location.y);
                return 0;
            }
            sliderHandle->setLocation(point.x, location.y);
            return 1;
        }
        return 0;
    };
    int gestureTapEvent(int x, int y) {
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        if (sprite->pointInSprite(point.x, point.y)) {
            float minWidth = sprite->getLocation().x - sprite->getWidth() / 2 + sliderHandle->getWidth() / 4;
            float maxWidth = sprite->getLocation().x + sprite->getWidth() / 2 - sliderHandle->getWidth() / 4;
            if (point.x < minWidth) {
                sliderHandle->setLocation(minWidth, location.y);
                return 0;
            }
            if (point.x > maxWidth) {
                sliderHandle->setLocation(maxWidth, location.y);
                return 0;
            }
            sliderHandle->setLocation(point.x, location.y);            
            return 1;
        }
        return 0;
    };
    int gestureLongTapEvent(int x, int y, float time) {
        return gestureTapEvent(x, y);
    };
private:
    Sprite* sliderHandle;
};

class Background: public Widget {
public:
    Background(int width, int height, Location location): Widget(width, height, location) {};
};

class Fruit: public Widget {
public:
    Fruit(int width, int height, Location location): Widget(width, height, location),
    alive(true), dies(false), dead(false) {
        //
    };
    int gestureTapEvent(int x, int y) {
        if (!alive) return false;
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        if (sprite->pointInSprite(point.x, point.y)) {
            if (clickFunction != NULL) clickFunction();
            return 1;
        }
        return 0;
    };  
    int gestureLongTapEvent(int x, int y, float time) {
        return gestureTapEvent(x, y);
    };
    void update() {
        if (alive) return;
        if (!dies && !dead) {
            sprite->setFrames(0, 5, 7.0f, false);
            dies = true;
            return;
        }
        if (dies && sprite->animationEnded()) dies = false;
        if (!dies && !dead) {
            TweenManager::getInstance()->addTween(sprite, TweenType::OPAQUE, 0.5f, Ease::Sinusoidal::InOut)
                ->target(0.0f)->remove(true)->start();
            dead = true;
            LOG_DEBUG("Fruit is dead.");
        }
    };
    bool alive;
    bool dies;
    bool dead;
};

class Scene: public InputListener {
public:
    Scene():
    created(false) {
        LOG_DEBUG("Scene created.");
    }
    virtual ~Scene() {
        LOG_DEBUG("Delete scene.");
        for (std::vector<Widget*>::iterator it = widgets.begin(); it < widgets.end(); ++it) {
            SAFE_DELETE(*it);
        }
        widgets.clear();
    };
    Button* addButton(const char* path, int width, int height, Location location) {
        LOG_INFO("Creating new Button widget.");
        Button* button = (Button*) new Button(width, height, location);
        button->setSprite(spriteBatch->registerSprite(path, width, height));
        button->spriteBatch = spriteBatch;
        widgets.push_back(button);
        return button;
    };
    CheckBox* addCheckBox(const char* path, int width, int height, Location location) {
        LOG_INFO("Creating new CheckBox widget.");
        CheckBox* checkBox = new CheckBox(width, height, location);
        checkBox->setSprite(spriteBatch->registerSprite(path, width, height));
        checkBox->spriteBatch = spriteBatch;
        widgets.push_back(checkBox);
        return checkBox;
    };
    Slider* addSlider(const char* path, int width, int height, Location location) {
        LOG_INFO("Creating new Slider widget.");
        Slider* slider = new Slider(width, height, location);
        slider->setSprite(spriteBatch->registerSprite(path, width, height));
        slider->spriteBatch = spriteBatch;
        widgets.push_back(slider);
        return slider;
    };
    Background* addBackground(const char* path, int width, int height, Location location) {
        LOG_INFO("Creating new Background widget.");
        Background* background = new Background(width, height, location);
        background->setSprite(spriteBatch->registerSprite(path, width, height));
        background->spriteBatch = spriteBatch;
        widgets.push_back(background);
        return background;
    };
    Fruit* addFruit(const char* path, int width, int height, Location location) {
        LOG_INFO("Creating new Fruit widget.");
        Fruit* fruit = new Fruit(width, height, location);
        fruit->setSprite(spriteBatch->registerSprite(path, width, height));
        fruit->spriteBatch = spriteBatch;
        widgets.push_back(fruit);
        return fruit;
    };
    void update() {
        for (std::vector<Widget*>::iterator it = widgets.begin(); it < widgets.end(); ++it) {
            (*it)->update();
        }
    };
    virtual status start(void) = 0;
    virtual void pause(void) {};
    virtual void resume(void) {};
    SpriteBatch* spriteBatch;
    bool created;
private:
    std::vector<Widget*> widgets;
};

#endif