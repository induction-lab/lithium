#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include "InputManager.h"
#include "GraphicsManager.h"
#include "SpriteBatch.h"

#include <functional>

class Interface;

class Widget: public InputListener {
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
    virtual void update() {};
    bool pointInWidget(Location location) {
        float halfWidth = mWidth * 0.5f;
        float halfHeight = mHeight * 0.5f;
        return ((location.x > mLocation.x - halfWidth ) &&
                (location.x < mLocation.x + halfWidth ) &&
                (location.y > mLocation.y - halfHeight) &&
                (location.y < mLocation.y + halfHeight));
    }
    void setDownFunction(std::function<void()> callback) {
        mDownFunction = callback;
    }
    void setUpFunction(std::function<void()> callback) {
        mUpFunction = callback;
    }
    void setClickFunction(std::function<void()> callback) {
        mClickFunction = callback;
    }
protected:
    friend class Interface;
    int32_t mWidth, mHeight;
    Location mLocation;
    std::function<void()> mDownFunction;
    std::function<void()> mUpFunction;
    std::function<void()> mClickFunction;
};

// For text justification
enum Justification {
    LEFT, MIDDLE, RIGHT
};

const int32_t CHAR_PADDING = 16;
class SpriteText : public Widget {
public:
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
            if (mJust == MIDDLE) {
                location.x = location.x - textWidth / 2;
            } else if (mJust == RIGHT) {
                location.x = location.x - textWidth;
            }
            sprite->setLocation(location);
        }
        if (mSpriteBatch->load() != STATUS_OK) return STATUS_ERROR;
		return STATUS_OK;
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
        Widget(width, height, location),
		mSprite(NULL)
	{
        //
    }
    void setSprite(Sprite* pSprite) {
        mSprite = pSprite;
        mSprite->setLocation(mLocation);
    }
	void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
		switch (event) {
		case Touch::TOUCH_PRESS:
			if (pointInWidget(Location(x, y))) {
				mSprite->setFrames(1, 1, 0.0f, false);
				if (mDownFunction != NULL) mDownFunction();
			}
			break;
		case Touch::TOUCH_RELEASE:
			mSprite->setFrames(0, 1, 0.0f, false);
			if (pointInWidget(Location(x, y))) {
				if (mUpFunction != NULL) mUpFunction();
			}
			break;
		case Touch::TOUCH_MOVE:
			if (!pointInWidget(Location(x, y))) {		
				mSprite->setFrames(0, 1, 0.0f, false);
			}
		}
	}
    void gestureTapEvent(int x, int y) {
        if (pointInWidget(Location(x, y))) {
			if (mClickFunction != NULL) mClickFunction();
		}
    };	
    void gestureLongTapEvent(int x, int y, float time) {
		gestureTapEvent(x, y);
    };
private:
    Sprite* mSprite;
};

class CheckBox: public Widget {
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
	void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
		switch (event) {
		case Touch::TOUCH_PRESS:
			if (pointInWidget(Location(x, y))) {
				if (mDownFunction != NULL) mDownFunction();				
			}
			break;
		case Touch::TOUCH_RELEASE:
			if (pointInWidget(Location(x, y))) {
				if (mUpFunction != NULL) mUpFunction();				
			}
			break;
		default:
			break;
		}
	}
    void gestureTapEvent(int x, int y) {
        if (pointInWidget(Location(x, y))) {
			mChecked = !mChecked;
			if (mChecked) {
				mSprite->setFrames(1, 1, 0.0f, false);
			} else {
				mSprite->setFrames(0, 1, 0.0f, false);
			}			
			if (mClickFunction != NULL) mClickFunction();	
		}
    }	
    void gestureLongTapEvent(int x, int y, float time) {
		gestureTapEvent(x, y);
    }
    bool getChecked() {
        return mChecked;
    }
    void setChecked(bool pChecked) {
		mChecked = pChecked;		
        if (mChecked) {
            mSprite->setFrames(1, 1, 0.0f, false);
        } else {
            mSprite->setFrames(0, 1, 0.0f, false);
        }
    }
private:
    Sprite* mSprite;
    bool mChecked;
};

class DragBox: public Widget {
public:
    DragBox(int32_t width, int32_t height, Location location):
        Widget(width, height, location),
        mSprite(NULL),
		mDraged(false),
		mDroped(false)
	{
        //
    }
    void setSprite(Sprite* pSprite) {
        mSprite = pSprite;
        mSprite->setLocation(mLocation);
		mStartLocation = mLocation;
    }
	void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
		switch (event) {
		case Touch::TOUCH_PRESS:
			if (pointInWidget(Location(x, y))) {
				mSprite->setFrames(1, 1, 0.0f, false);
				mDraged = true;
				mDroped = false;				
			}
			break;
		case Touch::TOUCH_RELEASE:
			if (pointInWidget(Location(x, y))) {
				mDraged = false;
				mSprite->setFrames(0, 1, 0.0f, false);				
			}
			break;
		default:
			break;			
		}
	}
	void gestureDragEvent(int x, int y) {
		if (mDraged) mSprite->setLocation(Location(x, y));
		mSprite->setFrames(0, 1, 0.0f, false);
	}
	void gestureDropEvent(int x, int y) {
		mDroped = true;
		mDraged = false;
	}
	void update() {
		if (mDroped) {
			Location location = mSprite->getLocation();
			float dx = (mStartLocation.x - location.x) / 100;
			float dy = (mStartLocation.y - location.y) / 100;
			location.x = location.x + dx;
			location.y = location.y + dy;
			mSprite->setLocation(location);
		}
	}
private:
    Sprite* mSprite;
	bool mDraged;
	bool mDroped;
	Location mStartLocation;
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
    void update() {
        if (mSprite->animationEnded()) mSprite->setFrames(27, 6, 8.0f, true);
    }
private:
    Sprite* mSprite;
};

class Background: public Widget {
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

class Interface {
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
    DragBox* addDragBox(const char* path, int32_t width, int32_t height, Location location) {
        LOG_INFO("Creating new DragBox widget");
        DragBox* dragBox = new DragBox(width, height, location);
        dragBox->setSprite(mSpriteBatch->registerSprite(path, width, height));
        mWidgets.push_back(dragBox);
        return dragBox;
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
    void update() {
        for (std::vector<Widget*>::iterator it = mWidgets.begin(); it < mWidgets.end(); ++it) {
            (*it)->update();
        }
	}
private:
    Interface(const Interface&);
    void operator=(const Interface&);
    SpriteBatch* mSpriteBatch;
    std::vector<Widget*> mWidgets;
};

#endif