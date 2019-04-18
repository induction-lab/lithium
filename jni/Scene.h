#ifndef __SCENE_H__
#define __SCENE_H__

#include "InputManager.h"
#include "GraphicsManager.h"
#include "SpriteBatch.h"

#include <functional>

// Base Widget.
class Widget: public InputListener {
public:
    Widget(int width, int height, Location location):
        width(width),
        height(height),
        location(location),
        sprite(NULL) {
        //
    };
    virtual void update() {};
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
};

// Button Widget.
class Button: public Widget {
public:
    Button(int width, int height, Location location):
        Widget(width, height, location),
        downFunction(NULL),
        upFunction(NULL),
        clickFunction(NULL) {
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
            if (inSprite && upFunction != NULL) upFunction();
            break;
        case Touch::TOUCH_MOVE:
            if (!inSprite) sprite->setFrames(0, 1, 0.0f, false);
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
    void setDownFunction(std::function<void()> callback)  { downFunction  = callback; };
    void setUpFunction(std::function<void()> callback)    { upFunction    = callback; };
    void setClickFunction(std::function<void()> callback) { clickFunction = callback; };    
private:
    std::function<void()> downFunction;
    std::function<void()> upFunction;
    std::function<void()> clickFunction;
};

// CheckBox Widget.
class CheckBox: public Widget {
public:
    CheckBox(int width, int height, Location location):
        Widget(width, height, location),
        checked(false),
        downFunction(NULL),
        upFunction(NULL),
        clickFunction(NULL) {
        //
    };
    void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        bool inSprite = sprite->pointInSprite(point.x, point.y);
        switch (event) {
        case Touch::TOUCH_DOWN:
            if (inSprite && downFunction != NULL) downFunction();               
            break;
        case Touch::TOUCH_UP:
            if (inSprite && upFunction != NULL) upFunction();
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
        if (checked) sprite->setFrames(1, 1, 0.0f, false);
        else sprite->setFrames(0, 1, 0.0f, false);
        if (clickFunction != NULL) clickFunction();
    };
    void setDownFunction(std::function<void()> callback)  { downFunction  = callback; };
    void setUpFunction(std::function<void()> callback)    { upFunction    = callback; };
    void setClickFunction(std::function<void()> callback) { clickFunction = callback; };
private:
    bool checked;
    std::function<void()> downFunction;
    std::function<void()> upFunction;
    std::function<void()> clickFunction;
};

// Slider Widget.
class Slider: public Widget {
public:
    Slider(int width, int height, Location location):
        Widget(width, height, location),
        precent(0),
        changed(false),
        slideFunction(NULL),
        sliderHandle(NULL),
        upFunction(NULL) {
        //
    };
    void setSliderHandle(const char* path, int width, int height, int position) {
        this->sliderHandle = spriteBatch->registerSprite(path, width, height);
        int x = location.x - sprite->getWidth() / 2;
        float minPosition = sprite->getLocation().x - sprite->getWidth() / 2 + sliderHandle->getWidth() / 4;
        sliderHandle->setLocation(minPosition, location.y);
    };
    void setPosition(int precent) {
        this->precent = (int)CLAMP(precent, 0.0f, 100.0f);
        float minPosition = sprite->getLocation().x - sprite->getWidth() / 2 + sliderHandle->getWidth() / 4;
        float maxPosition = sprite->getLocation().x + sprite->getWidth() / 2 - sliderHandle->getWidth() / 4;
        float position = (maxPosition - minPosition) * precent / 100.0f + minPosition;
        sliderHandle->setLocation(position, location.y);
    };
    void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        switch (event) {
        case Touch::TOUCH_UP:
            Location point = GraphicsManager::getInstance()->screenToRender(x, y);
            if ((sprite->pointInSprite(point.x, point.y) || changed) && upFunction != NULL) upFunction();
            break;
        }
    };
    int gestureTapEvent(int x, int y) {
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        if (sprite->pointInSprite(point.x, point.y)) {
            float minPosition = sprite->getLocation().x - sprite->getWidth() / 2 + sliderHandle->getWidth() / 4;
            float maxPosition = sprite->getLocation().x + sprite->getWidth() / 2 - sliderHandle->getWidth() / 4;
            if (point.x >= minPosition && point.x <= maxPosition) {
                sliderHandle->setLocation(point.x, location.y);
                float position = CLAMP((float)(point.x - minPosition) / (maxPosition - minPosition), 0.0f, 1.0f);
                int state = (int)CLAMP((float)(point.x - minPosition) / (maxPosition - minPosition) * 100.0f, 0.0f, 100.0f);
                changed = (state == precent);
                precent = state;
                if (slideFunction != NULL) slideFunction(precent);
                return 1;
            }
        }
        return 0;
    };
    int gestureLongTapEvent(int x, int y, float time) {
        return gestureTapEvent(x, y);
    };
    int gestureDragEvent(int x, int y) {
        float minPosition = sprite->getLocation().x - sprite->getWidth() / 2 + sliderHandle->getWidth() / 4;
        float maxPosition = sprite->getLocation().x + sprite->getWidth() / 2 - sliderHandle->getWidth() / 4;        
        float clampX = CLAMP(x, minPosition, maxPosition);
        return gestureTapEvent(x, y);
    };    
    int precent;
    void setSlideFunction(std::function<void(float)> callback) { slideFunction = callback; };
    void setUpFunction(std::function<void()> callback)         { upFunction    = callback; };
private:
    Sprite* sliderHandle;
    bool changed;
    std::function<void(int)> slideFunction;
    std::function<void()> upFunction;
};

// Background Widget.
class Background: public Widget {
public:
    Background(int width, int height, Location location):
        Widget(width, height, location) {
        //
    };
};

// Fruit Widget.
class Fruit: public Widget {
public:
    Fruit(int width, int height, Location location):
        Widget(width, height, location),
        alive(true),
        dies(false),
        dead(false),
        animated(false),
        selected(false),
        xScaleTween(NULL),
        yScaleTween(NULL),
        clickFunction(NULL) {
        //
    };
    int gestureTapEvent(int x, int y) {
        if (!alive) return false;
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        if (sprite->pointInSprite(point.x, point.y)) {
            if (!animated) {
                xScaleTween = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_X, 0.25f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
                yScaleTween = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_Y, 0.25f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
                animated = true;
            }
            if (clickFunction != NULL) clickFunction(this);
            return 1;
        }
        return 0;
    };  
    int gestureLongTapEvent(int x, int y, float time) {
        return gestureTapEvent(x, y);
    };
    void update() {
        if (animated && !selected) {
            TweenManager::getInstance()->remove(xScaleTween);
            TweenManager::getInstance()->remove(yScaleTween);
            xScaleTween = NULL;
            yScaleTween = NULL;
            sprite->scale = Vector2(0.9f, 0.9f);
            animated = false;
        }
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
    bool animated;
    bool selected;
    Tween* xScaleTween;
    Tween* yScaleTween;
    int indexI, indexJ;
    void setClickFunction(std::function<void(Fruit*)> callback) { clickFunction = callback; };
private:
    std::function<void(Fruit*)> clickFunction;
};

// Base Scene.
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
        LOG_INFO("Creating new 'Button' widget.");
        Button* button = (Button*) new Button(width, height, location);
        button->setSprite(spriteBatch->registerSprite(path, width, height));
        button->spriteBatch = spriteBatch;
        widgets.push_back(button);
        return button;
    };
    CheckBox* addCheckBox(const char* path, int width, int height, Location location) {
        LOG_INFO("Creating new 'CheckBox' widget.");
        CheckBox* checkBox = new CheckBox(width, height, location);
        checkBox->setSprite(spriteBatch->registerSprite(path, width, height));
        checkBox->spriteBatch = spriteBatch;
        widgets.push_back(checkBox);
        return checkBox;
    };
    Slider* addSlider(const char* path, int width, int height, Location location) {
        LOG_INFO("Creating new 'Slider' widget.");
        Slider* slider = new Slider(width, height, location);
        slider->setSprite(spriteBatch->registerSprite(path, width, height));
        slider->spriteBatch = spriteBatch;
        widgets.push_back(slider);
        return slider;
    };
    Background* addBackground(const char* path, int width, int height, Location location) {
        LOG_INFO("Creating new 'Background' widget.");
        Background* background = new Background(width, height, location);
        background->setSprite(spriteBatch->registerSprite(path, width, height));
        background->spriteBatch = spriteBatch;
        widgets.push_back(background);
        return background;
    };
    Fruit* addFruit(const char* path, int width, int height, Location location) {
        LOG_INFO("Creating new 'Fruit' widget.");
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