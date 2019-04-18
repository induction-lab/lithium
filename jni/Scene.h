#ifndef __SCENE_H__
#define __SCENE_H__

#include "InputManager.h"
#include "GraphicsManager.h"
#include "SpriteBatch.h"

#include <functional>

class Widget: public InputListener {
public:
    Widget(int32_t width, int32_t height, Location location):
        width(width), height(height), location(location),
        sprite(NULL),
        downFunction(NULL),
        upFunction(NULL),
        clickFunction(NULL) {
        //
    }
    virtual void update() {};
    bool pointInWidget(Location point) {
        point = GraphicsManager::getInstance()->screenToRender(point.x, point.y);
        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;
        return ((point.x > location.x - halfWidth ) &&
                (point.x < location.x + halfWidth ) &&
                (point.y > location.y - halfHeight) &&
                (point.y < location.y + halfHeight));
    }
    void setDownFunction(std::function<void()> callback) {
        downFunction = callback;
    }
    void setUpFunction(std::function<void()> callback) {
        upFunction = callback;
    }
    void setClickFunction(std::function<void()> callback) {
        clickFunction = callback;
    }
    void setSprite(Sprite* sprite) {
        this->sprite = sprite;
        sprite->setLocation(location.x, location.y);
    };
    Sprite* sprite;
protected:
    friend class Scene;
    int32_t width, height;
    Location location;
    std::function<void()> downFunction;
    std::function<void()> upFunction;
    std::function<void()> clickFunction;
};

class Button: public Widget {
public:
    Button(int32_t width, int32_t height, Location location): Widget(width, height, location) {
        //
    };
    Sprite* getSprite() {
        return sprite;
    };
    void setSprite(Sprite* pSprite) {
        sprite = pSprite;
        sprite->setLocation(location.x, location.y);
    };
    void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        switch (event) {
        case Touch::TOUCH_DOWN:
            if (pointInWidget(Location(x, y))) {
                sprite->setFrames(1, 1, 0.0f, false);
                if (downFunction != NULL) downFunction();
            }
            break;
        case Touch::TOUCH_UP:
            sprite->setFrames(0, 1, 0.0f, false);
            if (pointInWidget(Location(x, y))) {
                if (upFunction != NULL) upFunction();
            }
            break;
        case Touch::TOUCH_MOVE:
            if (!pointInWidget(Location(x, y))) {       
                sprite->setFrames(0, 1, 0.0f, false);
            }
        }
    };
    void gestureTapEvent(int x, int y) {
        if (pointInWidget(Location(x, y))) {
            if (clickFunction != NULL) clickFunction();
        }
    };  
    void gestureLongTapEvent(int x, int y, float time) {
        gestureTapEvent(x, y);
    };
};

class CheckBox: public Widget {
public:
    CheckBox(int32_t width, int32_t height, Location location): Widget(width, height, location),
        checked(false) {
        //
    };
    void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        switch (event) {
        case Touch::TOUCH_DOWN:
            if (pointInWidget(Location(x, y))) {
                if (downFunction != NULL) downFunction();               
            }
            break;
        case Touch::TOUCH_UP:
            if (pointInWidget(Location(x, y))) {
                if (upFunction != NULL) upFunction();
            }
            break;
        default:
            break;
        }
    };
    void gestureTapEvent(int x, int y) {
        if (pointInWidget(Location(x, y))) {
            checked = !checked;
            setChecked(checked);
        }
    };
    void gestureLongTapEvent(int x, int y, float time) {
        gestureTapEvent(x, y);
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

class Background: public Widget {
public:
    Background(int32_t width, int32_t height, Location location): Widget(width, height, location) {};
};

class Fruit: public Widget {
public:
    Fruit(int32_t width, int32_t height, Location location): Widget(width, height, location),
    alive(true), dies(false), dead(false) {
        //
    };
    void gestureTapEvent(int x, int y) {
        if (!alive) return;
        if (pointInWidget(Location(x, y))) {
            if (clickFunction != NULL) clickFunction();
        }
    };  
    void gestureLongTapEvent(int x, int y, float time) {
        gestureTapEvent(x, y);
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
    Button* addButton(const char* path, int32_t width, int32_t height, Location location) {
        LOG_INFO("Creating new Button widget.");
        Button* button = (Button*) new Button(width, height, location);
        button->setSprite(spriteBatch->registerSprite(path, width, height));
        widgets.push_back(button);
        return button;
    };
    CheckBox* addCheckBox(const char* path, int32_t width, int32_t height, Location location) {
        LOG_INFO("Creating new CheckBox widget.");
        CheckBox* checkBox = new CheckBox(width, height, location);
        checkBox->setSprite(spriteBatch->registerSprite(path, width, height));
        widgets.push_back(checkBox);
        return checkBox;
    };
    Background* addBackground(const char* path, int32_t width, int32_t height, Location location) {
        LOG_INFO("Creating new Background widget.");
        Background* background = new Background(width, height, location);
        background->setSprite(spriteBatch->registerSprite(path, width, height));
        widgets.push_back(background);
        return background;
    };
    Fruit* addFruit(const char* path, int32_t width, int32_t height, Location location) {
        LOG_INFO("Creating new Fruit widget.");
        Fruit* fruit = new Fruit(width, height, location);
        fruit->setSprite(spriteBatch->registerSprite(path, width, height));
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