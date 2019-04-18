#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <functional>
#include "SoundManager.h"
#include "GraphicsManager.h"
#include "SpriteBatch.h"

class Interface;

class Widget {
public:
    Widget(int32_t pWidth, int32_t pHeight, Location pLocation):
            mWidth(pWidth),
            mHeight(pHeight),
            mLocation(pLocation),
            mDownFunction(NULL),
            mUpFunction(NULL) {
        //
    }
    virtual void onEventDown(Location pLocation) {};
    virtual void onEventUp(Location pLocation) {};
    virtual void update() {};
    bool pointInWidget(Location pLocation) {
        float halfWidth = mWidth * 0.5f;
        float halfHeight = mHeight * 0.5f;
        return ((pLocation.x > mLocation.x - halfWidth ) &&
                (pLocation.x < mLocation.x + halfWidth ) &&
                (pLocation.y > mLocation.y - halfHeight) &&
                (pLocation.y < mLocation.y + halfHeight));
    }
    void setDownFunction(std::function<void()> callback) { mDownFunction = callback; }
    void setUpFunction(std::function<void()> callback) { mUpFunction = callback; }
protected:
    friend class Interface;
    int32_t mWidth, mHeight;
    Location mLocation;
    std::function<void()> mDownFunction;
    std::function<void()> mUpFunction;
};

// For text justification
enum class Justification {
    LEFT, MIDDLE, RIGHT
};

class SpriteText : public Widget {
public:
    const int32_t CHAR_PADDING = 16;
    SpriteText(const char* pPath, int32_t pWidth, int32_t pHeight, Location pLocation, Justification pJust, TimeManager* pTimeManager, GraphicsManager* pGraphicsManager):
            Widget(pWidth, pHeight, pLocation),
            mPath(pPath),
            mJust(pJust) {
        mSpriteBatch = new SpriteBatch(pTimeManager, pGraphicsManager);
    }
    ~SpriteText() {
        mSpriteBatch->clear();
        if (mSpriteBatch != NULL) delete mSpriteBatch;
        mSpriteBatch = NULL;
    }
    status setText(const char* pText) {
        mSpriteBatch->clear();
        int textLength = (int)strlen(pText);
        int textWidth = textLength * (mWidth - CHAR_PADDING);
        for (int count = 0; count <= textLength; count++) {
            Sprite* sprite = mSpriteBatch->registerSprite(mPath, mWidth, mHeight);
            sprite->setFrames(pText[count] , 1, 0.0f, false);
            Location location = Location(mLocation.x + (mWidth - CHAR_PADDING) * count, mLocation.y);
            if (mJust == Justification::MIDDLE) {
                location.x = location.x - textWidth / 2;
            } else if (mJust == Justification::RIGHT) {
                location.x = location.x - textWidth;
            }
            sprite->setLocation(location);
        }
        if (mSpriteBatch->load() != STATUS_OK) return STATUS_ERROR;
    }
    void onEventDown(Location pLocation) {}
    void onEventUp(Location pLocation) {}
private:
    SpriteBatch* mSpriteBatch;
    const char* mPath;
    Justification mJust;
};

class Button: public Widget {
public:
    Button(int32_t pWidth, int32_t pHeight, Location pLocation) :
            Widget(pWidth, pHeight, pLocation) {
        //
    }
    void setSprite(Sprite* pSprite) {
        mSprite = pSprite;
        mSprite->setLocation(mLocation);
    }
    void onEventDown(Location pLocation) {
        if (mPressed) return;
        mPressed = true;
        mSprite->setFrames(1, 1, 0.0f, false);
        if (mDownFunction != NULL) mDownFunction();
    }
    void onEventUp(Location pLocation) {
        if (!mPressed) return;
        mPressed = false;
        mSprite->setFrames(0, 1, 0.0f, false);
        if (mUpFunction != NULL) mUpFunction();
    };
private:
    Sprite* mSprite = NULL;
    bool mPressed = false;
};

class CheckBox : public Widget {
public:
    CheckBox(int32_t pWidth, int32_t pHeight, Location pLocation):
            Widget(pWidth, pHeight, pLocation),
            mSprite(NULL),
            mPressed(false),
            mChecked(false) {
        //
    }
    void setSprite(Sprite* pSprite) {
        mSprite = pSprite;
        mSprite->setLocation(mLocation);
    }
    void onEventDown(Location pLocation) {
        if (mPressed) return;
        mPressed = true;
        if (!mChecked)  {
            mSprite->setFrames(1, 1, 0.0f, false);
        } else {
            mSprite->setFrames(0, 1, 0.0f, false);
        }
        mChecked = !mChecked;
        if (mDownFunction != NULL) mDownFunction();
    }
    void onEventUp(Location pLocation) {
        if (!mPressed) return;
        mPressed = false;
        if (mUpFunction != NULL) mUpFunction();
    };
    bool getChecked() {        return mChecked;    }
    void setChecked( bool pChecked ) {
        if (pChecked) {
            mSprite->setFrames(1, 1, 0.0f, false);
            if (mDownFunction != NULL) mDownFunction();
        } else {
            mSprite->setFrames(0, 1, 0.0f, false);
            if (mUpFunction != NULL) mUpFunction();
        }
        mChecked = pChecked;
    }
private:
    Sprite* mSprite;
    bool mPressed;
    bool mChecked;
};

class Logo : public Widget {
public:
    Logo(int32_t pWidth, int32_t pHeight, Location pLocation):
            Widget(pWidth, pHeight, pLocation) {
        //
    }
    void setSprite(Sprite* pSprite) {
        mSprite = pSprite;
        mSprite->setLocation(mLocation);
        mSprite->setFrames(0, 27, 8.0f, false);
    }
    void update() {
        if (mSprite->animationEnded()) mSprite->setFrames(27, 6, 8.0f, true);
    }
private:
    Sprite* mSprite;
};

class Background : public Widget {
public:
    Background(int32_t pWidth, int32_t pHeight, Location pLocation):
            Widget(pWidth, pHeight, pLocation) {
        //
    }
    void setSprite(Sprite* pSprite) {
        mSprite = pSprite;
        mSprite->setLocation(mLocation);
    }
private:
    Sprite* mSprite;
};

class Interface {
public:
    Interface(TimeManager* pTimeManager, GraphicsManager* pGraphicsManager, InputManager* pInputManager) :
            mTimeManager(pTimeManager),
            mGraphicsManager(pGraphicsManager),
            mInputManager(pInputManager) {
        mSpriteBatch = new SpriteBatch(mTimeManager, mGraphicsManager);
    }
    ~Interface() {
        for (std::vector<Widget*>::iterator widgetIt = mWidgets.begin(); widgetIt < mWidgets.end(); ++widgetIt) {
            delete (*widgetIt);
        }
        mWidgets.clear();
        if (mSpriteBatch != NULL) delete mSpriteBatch;
        mSpriteBatch = NULL;
    }
    SpriteText* addSpriteText(const char* pPath, int32_t pWidth, int32_t pHeight, Location pLocation, Justification pJust) {
        LOG_INFO("Creating new SpriteText widget");
        SpriteText* spriteText = new SpriteText(pPath, pWidth, pHeight, pLocation, pJust, mTimeManager, mGraphicsManager);
        mWidgets.push_back(spriteText);
        return spriteText;
    }
    Button* addButton(const char* pPath, int32_t pWidth, int32_t pHeight, Location pLocation) {
        LOG_INFO("Creating new Button widget");
        Button* button = (Button*) new Button(pWidth, pHeight, pLocation);
        button->setSprite(mSpriteBatch->registerSprite(pPath, pWidth, pHeight));
        mWidgets.push_back(button);
        return button;
    }
    CheckBox* addCheckBox(const char* pPath, int32_t pWidth, int32_t pHeight, Location pLocation) {
        LOG_INFO("Creating new CheckBox widget");
        CheckBox* checkBox = new CheckBox(pWidth, pHeight, pLocation);
        checkBox->setSprite(mSpriteBatch->registerSprite(pPath, pWidth, pHeight));
        mWidgets.push_back(checkBox);
        return checkBox;
    }
    Logo* addLogo(const char* pPath, int32_t pWidth, int32_t pHeight, Location pLocation) {
        LOG_INFO("Creating new Logo widget");
        Logo* logo = new Logo(pWidth, pHeight, pLocation);
        logo->setSprite(mSpriteBatch->registerSprite(pPath, pWidth, pHeight));
        mWidgets.push_back(logo);
        return logo;
    }
    Background* addBackground(const char* pPath, int32_t pWidth, int32_t pHeight, Location pLocation) {
        LOG_INFO("Creating new Background widget");
        Background* background = new Background(pWidth, pHeight, pLocation);
        background->setSprite(mSpriteBatch->registerSprite(pPath, pWidth, pHeight));
        mWidgets.push_back(background);
        return background;
    }
    void update() {
        Location location(mInputManager->getDownX(), mInputManager->getDownY());
        for (std::vector<Widget*>::iterator widgetIt = mWidgets.begin(); widgetIt < mWidgets.end(); ++widgetIt) {
            (*widgetIt)->pointInWidget(location) ? (*widgetIt)->onEventDown(location) : (*widgetIt)->onEventUp(location);
            (*widgetIt)->update();
        }
    }
private:
    Interface(const Interface&);
    void operator=(const Interface&);
    SpriteBatch* mSpriteBatch;
    TimeManager* mTimeManager;
    GraphicsManager* mGraphicsManager;
    InputManager* mInputManager;
    std::vector<Widget*> mWidgets;
};


#endif