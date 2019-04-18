#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <functional>
#include "SoundManager.h"
#include "GraphicsManager.h"
#include "SpriteBatch.h"

class Interface;

class Widget {
public:
    Widget(int32_t width, int32_t height, Location location):
        mWidth(width),
        mHeight(height),
        mLocation(location),
        mDownFunction(NULL),
        mUpFunction(NULL),
		mClickFunction(NULL) {
        //
    }
    virtual void onEventDown() {};
    virtual void onEventUp() {};
	virtual void onEventClick() {};
    virtual void update() {};
    bool pointInWidget(Location location) {
        float halfWidth = mWidth * 0.5f;
        float halfHeight = mHeight * 0.5f;
        return ((location.x > mLocation.x - halfWidth ) &&
                (location.x < mLocation.x + halfWidth ) &&
                (location.y > mLocation.y - halfHeight) &&
                (location.y < mLocation.y + halfHeight));
    }
    void setDownFunction(std::function<void()> callback) { mDownFunction = callback; }
    void setUpFunction(std::function<void()> callback) { mUpFunction = callback; }
	void setClickFunction(std::function<void()> callback) { mClickFunction = callback; }
protected:
	bool mTouched = false;
    friend class Interface;
    int32_t mWidth, mHeight;
    Location mLocation;
    std::function<void()> mDownFunction;
    std::function<void()> mUpFunction;
    std::function<void()> mClickFunction;	
};

// For text justification
enum class Justification {
    LEFT, MIDDLE, RIGHT
};

class SpriteText : public Widget {
public:
    const int32_t CHAR_PADDING = 16;
    SpriteText(const char* path, int32_t width, int32_t height, Location location, Justification just):
            Widget(width, height, location),
            mPath(path),
            mJust(just) {
        mSpriteBatch = new SpriteBatch();
    }
    ~SpriteText() {
        mSpriteBatch->clear();
        if (mSpriteBatch != NULL) delete mSpriteBatch;
        mSpriteBatch = NULL;
    }
    status setText(const char* text) {
        mSpriteBatch->clear();
        int textLength = (int)strlen(text);
        int textWidth = textLength * (mWidth - CHAR_PADDING);
        for (int count = 0; count <= textLength; count++) {
            Sprite* sprite = mSpriteBatch->registerSprite(mPath, mWidth, mHeight);
            sprite->setFrames(text[count] , 1, 0.0f, false);
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
    void onEventDown(Location location) {}
    void onEventUp(Location location) {}
private:
    SpriteBatch* mSpriteBatch;
    const char* mPath;
    Justification mJust;
};

class Button: public Widget {
public:
    Button(int32_t width, int32_t height, Location location):
        Widget(width, height, location) {
        //
    }
    void setSprite(Sprite* pSprite) {
        mSprite = pSprite;
        mSprite->setLocation(mLocation);
    }
    void onEventDown() {
        mSprite->setFrames(1, 1, 0.0f, false);
        if (mDownFunction != NULL) mDownFunction();
    }
    void onEventUp() {
        mSprite->setFrames(0, 1, 0.0f, false);
        if (mUpFunction != NULL) mUpFunction();
    };
	void onEventClick() {
		if (mClickFunction != NULL) mClickFunction();
	}
private:
    Sprite* mSprite = NULL;
};

class CheckBox : public Widget {
public:
    CheckBox(int32_t width, int32_t height, Location location):
        Widget(width, height, location),
        mSprite(NULL),
        mChecked(false) {
        //
    }
    void setSprite(Sprite* pSprite) {
        mSprite = pSprite;
        mSprite->setLocation(mLocation);
    }
    void onEventDown() {
        if (mChecked)  {
            mSprite->setFrames(1, 1, 0.0f, false);
        } else {
            mSprite->setFrames(0, 1, 0.0f, false);
        }		
        if (mDownFunction != NULL) mDownFunction();
    }
    void onEventUp() {
        if (mChecked)  {
            mSprite->setFrames(1, 1, 0.0f, false);
        } else {
            mSprite->setFrames(0, 1, 0.0f, false);
        }
        if (mUpFunction != NULL) mUpFunction();
    };
	void onEventClick() {
        mChecked = !mChecked;
        if (!mChecked)  {
            mSprite->setFrames(1, 1, 0.0f, false);
        } else {
            mSprite->setFrames(0, 1, 0.0f, false);
        }		
		if (mClickFunction != NULL) mClickFunction();
	}
    bool getChecked() { 
		return mChecked;
	}
    void setChecked( bool pChecked ) {
        if (pChecked) {
            mSprite->setFrames(1, 1, 0.0f, false);
        } else {
            mSprite->setFrames(0, 1, 0.0f, false);
        }
        mChecked = pChecked;
    }
private:
    Sprite* mSprite;
    bool mChecked;
};

class Logo: public Widget {
public:
    Logo(int32_t width, int32_t height, Location location):
        Widget(width, height, location) {
        //
    }
    void setSprite(Sprite* sprite) {
        mSprite = sprite;
        mSprite->setLocation(mLocation);
        mSprite->setFrames(0, 27, 8.0f, false);
    }
    void update() { // !!!!!
        if (mSprite->animationEnded()) mSprite->setFrames(27, 6, 8.0f, true);
    }
private:
    Sprite* mSprite;
};

class Background : public Widget {
public:
    Background(int32_t width, int32_t height, Location location):
        Widget(width, height, location) {
        //
    }
    void setSprite(Sprite* sprite) {
        mSprite = sprite;
        mSprite->setLocation(mLocation);
    }
private:
    Sprite* mSprite;
};

class Interface: public InputListener {
public:
    Interface() {
        mSpriteBatch = new SpriteBatch();
    }
    ~Interface() {
        for (std::vector<Widget*>::iterator it = mWidgets.begin(); it < mWidgets.end(); ++it) {
            delete (*it);
        }
        mWidgets.clear();
        if (mSpriteBatch != NULL) delete mSpriteBatch;
        mSpriteBatch = NULL;
    }
    SpriteText* addSpriteText(const char* path, int32_t width, int32_t height, Location location, Justification just) {
        LOG_INFO("Creating new SpriteText widget");
        SpriteText* spriteText = new SpriteText(path, width, height, location, just);
        mWidgets.push_back(spriteText);
        return spriteText;
    }
    Button* addButton(const char* path, int32_t width, int32_t height, Location location) {
        LOG_INFO("Creating new Button widget");
        Button* button = (Button*) new Button(width, height, location);
        button->setSprite(mSpriteBatch->registerSprite(path, width, height));
        mWidgets.push_back(button);
        return button;
    }
    CheckBox* addCheckBox(const char* path, int32_t width, int32_t height, Location location) {
        LOG_INFO("Creating new CheckBox widget");
        CheckBox* checkBox = new CheckBox(width, height, location);
        checkBox->setSprite(mSpriteBatch->registerSprite(path, width, height));
        mWidgets.push_back(checkBox);
        return checkBox;
    }
    Logo* addLogo(const char* path, int32_t width, int32_t height, Location location) {
        LOG_INFO("Creating new Logo widget");
        Logo* logo = new Logo(width, height, location);
        logo->setSprite(mSpriteBatch->registerSprite(path, width, height));
        mWidgets.push_back(logo);
        return logo;
    }
    Background* addBackground(const char* path, int32_t width, int32_t height, Location location) {
        LOG_INFO("Creating new Background widget");
        Background* background = new Background(width, height, location);
        background->setSprite(mSpriteBatch->registerSprite(path, width, height));
        mWidgets.push_back(background);
        return background;
    }
	void touchPressEvent(int x, int y, size_t pointerId) {
		for (std::vector<Widget*>::iterator it = mWidgets.begin(); it < mWidgets.end(); ++it) {
			if ((*it)->pointInWidget(Location(x, y))) {
				(*it)->onEventDown();
				(*it)->mTouched = true;
			} 
		}
	};
	void touchReleaseEvent(int x, int y, size_t pointerId) {
		for (std::vector<Widget*>::iterator it = mWidgets.begin(); it < mWidgets.end(); ++it) {
			if ((*it)->pointInWidget(Location(x, y))) {
				(*it)->onEventUp();
				(*it)->mTouched = false;
			} 
		}
	};
	void touchMoveEvent(int x, int y, size_t pointerId) {
		for (std::vector<Widget*>::iterator it = mWidgets.begin(); it < mWidgets.end(); ++it) {
			if (!(*it)->pointInWidget(Location(x, y)) && (*it)->mTouched) {
				(*it)->onEventUp();
				(*it)->mTouched = false;
			} 
		}
	};
	void gestureTapEvent(int x, int y) {
		for (std::vector<Widget*>::iterator it = mWidgets.begin(); it < mWidgets.end(); ++it) {
			if ((*it)->pointInWidget(Location(x, y))) {
				(*it)->onEventClick();
			} 
		}
	};
	void gestureLongTapEvent(int x, int y, float time) {
		/// LOG_INFO("gestureLongTapEvent %d %d %f", x, y, time);
		for (std::vector<Widget*>::iterator it = mWidgets.begin(); it < mWidgets.end(); ++it) {
			if ((*it)->pointInWidget(Location(x, y))) {
				(*it)->onEventClick();
			} 
		}
	};
private:
    Interface(const Interface&);
    void operator=(const Interface&);
    SpriteBatch* mSpriteBatch;
    std::vector<Widget*> mWidgets;
};

#endif