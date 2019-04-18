#ifndef __SCENE_H__
#define __SCENE_H__

#include <functional>

#include "InputManager.h"
#include "GraphicsManager.h"
#include "SpriteBatch.h"

// Base Widget.
class Widget: public InputListener {
public:
    Widget():
        dead(false),
        sprite(NULL),
        spriteBatch(NULL) {
        // LOG_DEBUG("Create widget.");
    };
    ~Widget() {
        // LOG_DEBUG("Delete widget.");
    }
    virtual void update() {};
    void setSprite(Sprite* sprite, Vector2 location) {
        this->sprite = sprite;
        sprite->location = location;
    };
    virtual void onDead(Tweenable* t) {
        dead = true;
    };    
    bool dead;    
    Sprite* sprite;
protected:
    friend class Scene;
    SpriteBatch* spriteBatch;
};

#include "RasterFont.h"

// Button Widget.
class Button: public Widget {
public:
    Button():
        downFunction(NULL),
        upFunction(NULL),
        clickFunction(NULL) {
        //
    };
    void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        Vector2 point = GraphicsManager::getInstance()->screenToRender(x, y);
        bool inSprite = sprite->pointInSprite(point.x, point.y);
        switch (event) {
            case Touch::TOUCH_DOWN:
                if (inSprite) {
                    sprite->setFrame(1);
                    if (downFunction != NULL) downFunction();
                }
                break;
            case Touch::TOUCH_UP:
                sprite->setFrame(0);
                if (inSprite && upFunction != NULL) upFunction();
                break;
            case Touch::TOUCH_MOVE:
                if (!inSprite) sprite->setFrame(0);
        }
    };
    int gestureTapEvent(int x, int y) {
        Vector2 point = GraphicsManager::getInstance()->screenToRender(x, y);
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
    void setDownFunction(std::function<void()> callback) { downFunction = callback; };
    void setUpFunction(std::function<void()> callback) { upFunction = callback; };
    void setClickFunction(std::function<void()> callback) { clickFunction = callback; };    
private:
    std::function<void()> downFunction;
    std::function<void()> upFunction;
    std::function<void()> clickFunction;
};

// CheckBox Widget.
class CheckBox: public Widget {
public:
    CheckBox():
        checked(false),
        downFunction(NULL),
        upFunction(NULL),
        clickFunction(NULL) {
        //
    };
    void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        Vector2 point = GraphicsManager::getInstance()->screenToRender(x, y);
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
        Vector2 point = GraphicsManager::getInstance()->screenToRender(x, y);
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
        if (checked) sprite->setFrame(1);
        else sprite->setFrame(0);
        if (clickFunction != NULL) clickFunction();
    };
    void setDownFunction(std::function<void()> callback)  { downFunction = callback; };
    void setUpFunction(std::function<void()> callback) { upFunction = callback; };
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
    Slider():
        precent(0),
        changed(false),
        slideFunction(NULL),
        sliderHandle(NULL),
        upFunction(NULL) {
        //
    };
    void setSliderHandle(const char* path, int width, int height, int position) {
        sliderHandle = spriteBatch->registerSprite(path, width, height);
        int x = sprite->location.x - sprite->getWidth() / 2;
        float minPosition = sprite->location.x - sprite->getWidth() / 2 + sliderHandle->getWidth() / 4;
        sliderHandle->location = Vector2(minPosition, sprite->location.y);
    };
    void setPosition(int precent) {
        this->precent = (int)CLAMP(precent, 0.0f, 100.0f);
        float minPosition = sprite->location.x - sprite->getWidth() / 2 + sliderHandle->getWidth() / 4;
        float maxPosition = sprite->location.x + sprite->getWidth() / 2 - sliderHandle->getWidth() / 4;
        float position = (maxPosition - minPosition) * precent / 100.0f + minPosition;
        sliderHandle->location = Vector2(position, sprite->location.y);
    };
    void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        switch (event) {
        case Touch::TOUCH_UP:
            Vector2 point = GraphicsManager::getInstance()->screenToRender(x, y);
            if ((sprite->pointInSprite(point.x, point.y) || changed) && upFunction != NULL) upFunction();
            break;
        }
    };
    int gestureTapEvent(int x, int y) {
        Vector2 point = GraphicsManager::getInstance()->screenToRender(x, y);
        if (sprite->pointInSprite(point.x, point.y)) {
            float minPosition = sprite->location.x - sprite->getWidth() / 2 + sliderHandle->getWidth() / 4;
            float maxPosition = sprite->location.x + sprite->getWidth() / 2 - sliderHandle->getWidth() / 4;
            if (point.x >= minPosition && point.x <= maxPosition) {
                sliderHandle->location = Vector2(point.x, sprite->location.y);
                float position = CLAMP((float)(point.x - minPosition) / (maxPosition - minPosition), 0.0f, 1.0f);
                int state = (int)CLAMP((float)(point.x - minPosition) / (maxPosition - minPosition) * 100.0f, 0.0f, 100.0f);
                changed = (state == precent);
                precent = state;
                if (slideFunction != NULL) slideFunction(precent);
                changed = false;
                return 1;
            }
        }
        return 0;
    };
    int gestureLongTapEvent(int x, int y, float time) {
        return gestureTapEvent(x, y);
    };
    int gestureDragEvent(int x, int y) {
        return gestureTapEvent(x, y);
    };
    bool getChanged() {
        return changed;
    };
    int precent;
    void setSlideFunction(std::function<void(float)> callback) { slideFunction = callback; };
    void setUpFunction(std::function<void()> callback) { upFunction = callback; };
private:
    Sprite* sliderHandle;
    bool changed;
    std::function<void(int)> slideFunction;
    std::function<void()> upFunction;
};

// Background Widget.
class Background: public Widget {
public:
    Background():
        clickFunction(NULL) {
        //
    };
    int gestureTapEvent(int x, int y) {
        Vector2 point = GraphicsManager::getInstance()->screenToRender(x, y);
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
    void setClickFunction(std::function<void()> callback) { clickFunction = callback; };    
private:
    std::function<void()> clickFunction;
};

// Animation class.
class Animation: public Widget {
public:
    Animation(int frames, float duration, float delay):
        frames(frames),
        duration(duration),
        delay(delay),
        started(false) {
        //
    };
    void update() {
        if (!started) {
            TweenManager::getInstance()->addTween(sprite, TweenType::FRAME, duration, Ease::Linear)->target(frames)->remove(true)
                ->onComplete(std::bind(&Widget::onDead, this, std::placeholders::_1))
                ->start(delay);
            started = true;
        }
    };
    int frames;
    float duration;
    float delay;
    bool started;
};

// BonusText class.
class BonusText: public Widget {
public:
    BonusText(int frame):
        started(false) {
        //
    };
    void update() {
        if (!started) {
            Tween* t1 = TweenManager::getInstance()->addTween(sprite, TweenType::OPAQUE, 0.25f, Ease::Sinusoidal::InOut)->target(1.0f)->remove(true)->start();
            Tween* t2 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_XY, 0.25f, Ease::Back::Out)->target(1.0f, 1.0f)->remove(true);
            Tween* t3 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_XY, 0.5f, Ease::Back::Out)->target(0.5f, 0.5f)->remove(true)->delay(0.5f);
            Tween* t4 = TweenManager::getInstance()->addTween(sprite, TweenType::OPAQUE, 0.5f, Ease::Sinusoidal::InOut)->target(0.0f)->remove(true)->delay(0.5f);
            t2->addChain(t3);
            t2->addChain(t4);
            t3->onComplete(std::bind(&Widget::onDead, this, std::placeholders::_1));
            t2->start();
            started = true;
        }
    };
    void onDead(Tweenable* t) {
        // !!! LOG_DEBUG("!!! %s", ((Sprite*)(t))->texturePath);
        Widget::onDead(t);
        // ??? ...
    };
    bool started;
};

// Base Scene.
class Scene: public InputListener {
public:
    Scene():
    created(false) {
        LOG_DEBUG("Create scene.");
    }
    virtual ~Scene() {
        LOG_DEBUG("Delete scene.");
        LOG_DEBUG("Delete %d widgets.", widgets.size());
        for (std::vector<Widget*>::iterator it = widgets.begin(); it < widgets.end(); ++it) {
            SAFE_DELETE(*it);
        }
        widgets.clear();
    };
    Button* addButton(const char* path, int width, int height, Vector2 location) {
        LOG_INFO("Creating new 'Button' widget.");
        Button* button = (Button*) new Button();
        button->setSprite(spriteBatch->registerSprite(path, width, height), location);
        button->spriteBatch = spriteBatch;
        widgets.push_back(button);
        return button;
    };
    CheckBox* addCheckBox(const char* path, int width, int height, Vector2 location) {
        LOG_INFO("Creating new 'CheckBox' widget.");
        CheckBox* checkBox = new CheckBox();
        checkBox->setSprite(spriteBatch->registerSprite(path, width, height), location);
        checkBox->spriteBatch = spriteBatch;
        widgets.push_back(checkBox);
        return checkBox;
    };
    Slider* addSlider(const char* path, int width, int height, Vector2 location) {
        LOG_INFO("Creating new 'Slider' widget.");
        Slider* slider = new Slider();
        slider->setSprite(spriteBatch->registerSprite(path, width, height), location);
        slider->spriteBatch = spriteBatch;
        widgets.push_back(slider);
        return slider;
    };
    Background* addBackground(const char* path, int width, int height, Vector2 location) {
        LOG_INFO("Creating new 'Background' widget.");
        Background* background = new Background();
        background->setSprite(spriteBatch->registerSprite(path, width, height), location);
        background->spriteBatch = spriteBatch;
        widgets.push_back(background);
        return background;
    };
    Animation* addAnimation(const char* path, int width, int height, Vector2 location, int frames, float duration, float delay = 0.0f) {
        LOG_DEBUG("Creating new 'Animation' widget.");
        Animation* animation = new Animation(frames, duration, delay);
        animation->setSprite(spriteBatch->registerSprite(path, width, height), location);
        animation->sprite->order = 1;
        animation->spriteBatch = spriteBatch;
        widgets.push_back(animation);
        return animation;
    };
    BonusText* addBonusText(const char* path, int width, int height, Vector2 location, int frame) {
        LOG_DEBUG("Creating new 'BonusText' widget.");
        BonusText* bonusText = new BonusText(frame);
        bonusText->setSprite(spriteBatch->registerSprite(path, width, height), location);
        bonusText->sprite->order = 1;
        bonusText->sprite->scale = Vector2(0.5f, 0.5f);
        bonusText->sprite->opaque = 0.0f;
        bonusText->sprite->setFrame(frame);            
        bonusText->spriteBatch = spriteBatch;
        widgets.push_back(bonusText);
        return bonusText;
    };
    RasterFont* addRasterFont(const char* path, int width, int height, Vector2 location, Justification just, TextAnimation animation = TextAnimation::NONE) {
        RasterFont* rasterFont = new RasterFont(path, width, height, location, just, animation);
        rasterFont->spriteBatch = spriteBatch;
        widgets.push_back(rasterFont);
        return rasterFont;
    };
    virtual void update() {
        for (std::vector<Widget*>::const_reverse_iterator it = widgets.rbegin(); it < widgets.rend(); ++it) {
            (*it)->update();
            if ((*it)->dead) {
                // Delete dead widget.
                LOG_DEBUG("Delete dead widget.");
                spriteBatch->unregisterSprite((*it)->sprite);
                widgets.erase(std::remove(widgets.begin(), widgets.end(), *it), widgets.end());
            }
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

#endif // __SCENE_H__