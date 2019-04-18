#ifndef __RASTERFONT_H__
#define __RASTERFONT_H__

#include "SpriteBatch.h"

// For text justification
enum class Justification {
    LEFT, MIDDLE, RIGHT
};

class RasterFont {
public:
    const int CHAR_PADDING = 44;
    RasterFont(const char* path, int width, int height, Vector2 location, Justification just):
        path(path), width(width), height(height), location(location),
        newText("\0"),
        lastText("\0"),
        animated(0),
        just(just) {
        // LOG_DEBUG("Create RasterFont.")
        spriteBatch = new SpriteBatch();
    };
    ~RasterFont() {
        // LOG_DEBUG("Delete RasterFont.")
        sprites.clear();
    };
    void update() {
        if (animated > 0) return;
        if (newText.compare(lastText) == 0) return;
        const char* text = newText.c_str();
        int lastTextLength = strlen(lastText.c_str());
        int textLength = strlen(text);
        int textWidth = textLength * (width - CHAR_PADDING);
        // Recreate only changed sprite.
        for (int count = 0; count < textLength; count++) {
            // Test if char are not equal.
            bool q = ((count < lastTextLength) && (text[count] != lastText.c_str()[count]));
            if (q) spriteBatch->unregisterSprite(sprites.at(count));
            // Get shift.
            Vector2 l = Vector2(location.x + (width - CHAR_PADDING) * count + 5, location.y + frand(4) - 2.0f);
            if (just == Justification::MIDDLE) {
                l.x = l.x - textWidth / 2;
            } else if (just == Justification::RIGHT) {
                l.x = l.x - textWidth;
            }
            // Create new sprite.
            if (q || (count >= lastTextLength)) {
                Sprite* sprite = spriteBatch->registerSprite(path, width, height);
                sprite->setFrame(text[count]);
                sprite->location = l;
                animated++;
                Tween* t1 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_XY, 0.1f, Ease::Sinusoidal::InOut)
                    ->target(1.5f, 1.5f)->remove(true)->onComplete(std::bind(&RasterFont::onAnimatedComplete, this));
                animated++;
                Tween* t2 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_XY, 0.25f, Ease::Sinusoidal::InOut)
                    ->target(1.0f, 1.0f)->remove(true)->onComplete(std::bind(&RasterFont::onAnimatedComplete, this));;
                t1->addChain(t2)->start();
                if (q) {
                    sprites.at(count) = sprite;
                } else {
                    sprites.push_back(sprite);
                }
            } else sprites.at(count)->location.x = l.x;
        }
        // Delete tail.
        if (lastTextLength > textLength) {
            for (int count = textLength; count < lastTextLength; count++) spriteBatch->unregisterSprite(sprites.at(count));
            sprites.erase(sprites.begin() + textLength, sprites.end());
        }
        lastText = text;
    }
    void onAnimatedComplete() {
        animated--;
    }
    void setText(const char* text) {
        newText = text;
    };
private:
    int width, height;
    Vector2 location;
    SpriteBatch* spriteBatch;
    const char* path;
    std::string newText;
    std::string lastText;
    int animated;
    std::vector<Sprite*> sprites;
    Justification just;
};

#endif // __RASTERFONT_H__