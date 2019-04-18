#ifndef __RASTERFONT_H__
#define __RASTERFONT_H__

#include "SpriteBatch.h"

// For text justification
enum class Justification {
    LEFT, MIDDLE, RIGHT
};

class RasterFont {
public:
    const int CHAR_PADDING = 16;
    RasterFont(const char* path, int width, int height, Vector2 location, Justification just):
        path(path), width(width), height(height), location(location),
        lastText("\0"),
        just(just) {
        spriteBatch = new SpriteBatch();
    };
    ~RasterFont() {
        sprites.clear();
    };
    status setText(const char* text) {
        // spriteBatch->reset();
        int lastTextLength = strlen(lastText.c_str());
        int textLength = strlen(text);
        int textWidth = textLength * (width - CHAR_PADDING);
        // Recreate only changed sprite.
        for (int count = 0; count < textLength; count++) {
            LOG_WARN("count=%d sprites=%d", count, sprites.size());
            // Test if char are not equal.
            LOG_WARN("%s %s", text, lastText.c_str());
            bool q = ((count < lastTextLength) && (text[count] != lastText.c_str()[count]));
            if (q) {
                LOG_INFO("deletw");
                spriteBatch->unregisterSprite(sprites.at(count));
            }
            // Get shift.
            Vector2 l = Vector2(location.x + (width - CHAR_PADDING) * count, location.y);
            if (just == Justification::MIDDLE) {
                l.x = l.x - textWidth / 2;
            } else if (just == Justification::RIGHT) {
                l.x = l.x - textWidth;
            }
            if (q || (count >= lastTextLength)) {
                LOG_INFO("create");
                Sprite* sprite = spriteBatch->registerSprite(path, width, height);
                sprite->setFrame(text[count]);
                sprite->location = l;
                Tween* t1 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_XY, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.5f, 1.5f)->remove(true);
                Tween* t2 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_XY, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.0f, 1.0f)->remove(true);
                t1->addChain(t2)->start();
                if (q) {
                    LOG_INFO("update");
                    sprites.at(count) = sprite;
                } else {
                    LOG_INFO("add");
                    sprites.push_back(sprite);
                }
                LOG_DEBUG("count=%d sprites=%d", count, sprites.size());
            } else sprites.at(count)->location = l;
        }
        // Delete tail.
        if (lastTextLength > textLength) {
            for (int count = textLength; count < lastTextLength; count++) {
                LOG_INFO("del tail %d", count);
                spriteBatch->unregisterSprite(sprites.at(count));
            }
            sprites.erase(sprites.begin() + textLength, sprites.end());
        }
        LOG_DEBUG("!!! sprites=%d", sprites.size());
        lastText = text;
        return STATUS_OK;
    };
private:
    int width, height;
    Vector2 location;
    SpriteBatch* spriteBatch;
    const char* path;
    std::string lastText;
    std::vector<Sprite*> sprites;
    Justification just;
};

    /*
    RasterFont* addRasterFont(const char* path, int pWidth, int pHeight, Location pLocation, Justification pJust) {
        LOG_INFO("Creating new RasterFont widget");
        RasterFont* spriteText = new RasterFont(path, pWidth, pHeight, pLocation, pJust, mTimeManager, mGraphicsManager);
        mWidgets.push_back(spriteText);
        return spriteText;
    }
    */

#endif // __RASTERFONT_H__