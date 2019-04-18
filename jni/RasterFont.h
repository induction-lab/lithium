#ifndef __RASTERFONT_H__
#define __RASTERFONT_H__

#include "SpriteBatch.h"

// For text justification.
enum class Justification {
    LEFT, MIDDLE, RIGHT
};

// Text animation.
enum class TextAnimation {
    NONE, SCALE, SLIDE
};

class RasterFont: public Widget {
public:
    const int CHAR_PADDING = 44;
    RasterFont(const char* path, int width, int height, Vector2 location, Justification just, TextAnimation animation = TextAnimation::NONE):
        path(path), width(width), height(height), location(location),
        newText("\0"),
        lastText("\0"),
        textChanged(false),
        startedTweens(0),
        animation(animation),
        scale(Vector2(1.0f, 1.0f)),
        just(just) {
        // LOG_DEBUG("Create RasterFont.")
    };
    ~RasterFont() {
        // LOG_DEBUG("Delete RasterFont.")
        sprites.clear();
    };
    void update() {
        if (animation == TextAnimation::SCALE) {
            if (startedTweens > 0) return;
            if (newText.compare(lastText) == 0 && !textChanged) return;
            const char* text = newText.c_str();
            int lastTextLength = strlen(lastText.c_str());
            int textLength = strlen(text);
            int textWidth = textLength * (width - CHAR_PADDING);
            // Recreate only changed sprite.
            for (int count = 0; count < textLength; count++) {
                // Test if char changed.
                bool q = ((count < lastTextLength) && (text[count] != lastText.c_str()[count]));
                if (q) spriteBatch->unregisterSprite(sprites.at(count));
                // Get shift.
                Vector2 l = Vector2(location.x + (width - CHAR_PADDING) * count * scale.x, location.y + frand(4) - 2.0f);
                if (just == Justification::MIDDLE) l.x = l.x - textWidth / 2 * scale.x;
                else if (just == Justification::RIGHT) l.x = l.x - textWidth * scale.x;
                // Create new sprite.
                if (q || (count >= lastTextLength)) {
                    Sprite* sprite = spriteBatch->registerSprite(path, width, height);
                    sprite->setFrame(text[count]);
                    sprite->location = l;
                    sprite->scale = scale;
                    // Add animation.
                    Tween* t1 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_XY, 0.1f, Ease::Sinusoidal::InOut)
                        ->target(1.5f, 1.5f)->remove(true);
                    Tween* t2 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_XY, 0.25f, Ease::Sinusoidal::InOut)
                        ->target(1.0f, 1.0f)->remove(true)
                        ->onComplete(std::bind(&RasterFont::onAnimatedComplete, this, std::placeholders::_1));
                    startedTweens++;
                    t1->addChain(t2)->start();
                    if (q) sprites.at(count) = sprite;
                    else sprites.push_back(sprite);
                } else sprites.at(count)->location.x = l.x;
            }
            // Delete tail.
            if (lastTextLength > textLength) {
                for (int count = textLength; count < lastTextLength; count++) spriteBatch->unregisterSprite(sprites.at(count));
                sprites.erase(sprites.begin() + textLength, sprites.end());
            }
            lastText = text;
            textChanged = false;
        }
    };
    void onAnimatedComplete(Tweenable* t) {
        switch (animation) {
            case TextAnimation::SLIDE: {
                spriteBatch->unregisterSprite((Sprite*)t);
                break;
            }
            case TextAnimation::SCALE: {
                startedTweens--;
                break;
            }
        }
    };
    void setText(const char* text) {
        // Slide animation.
        if (animation == TextAnimation::SLIDE) {
            int textLength = strlen(text);
            int textWidth = textLength * (width - CHAR_PADDING);
            for (int count = 0; count < textLength; count++) {
                // Get shift.
                Vector2 l = Vector2(location.x + (width - CHAR_PADDING) * count * scale.x, location.y + frand(4) - 2.0f);
                if (just == Justification::MIDDLE) l.x = l.x - textWidth / 2 * scale.x;
                else if (just == Justification::RIGHT) l.x = l.x - textWidth * scale.x;            
                // Create new sprite.
                Sprite* sprite = spriteBatch->registerSprite(path, width, height);
                sprite->setFrame(text[count]);
                sprite->location = l;
                sprite->scale = scale;
                sprite->order = (text[count] == 43)? 1 : 3; // set "+" to back
                sprite->opaque = 0.0f;
                // Add animation.
                Tween* t1 = TweenManager::getInstance()->addTween(sprite, TweenType::OPAQUE, 0.15f, Ease::Sinusoidal::InOut)
                    ->target(1.0f)->remove(true)->start();                
                Tween* t2 = TweenManager::getInstance()->addTween(sprite, TweenType::POSITION_Y, 0.25f, Ease::Back::Out)
                    ->target(location.y + 21.0f)->remove(true);
                Tween* t3 = TweenManager::getInstance()->addTween(sprite, TweenType::OPAQUE, 0.5f, Ease::Sinusoidal::InOut)
                    ->target(0.0f)->remove(true)->delay(0.5f)
                    ->onComplete(std::bind(&RasterFont::onAnimatedComplete, this, std::placeholders::_1));
                startedTweens++;
                t2->addChain(t3);
                t2->start();
            }
        }
        newText = text;
        textChanged = true;
    };
    Vector2 scale;
private:
    int width, height;
    Vector2 location;
    Vector2 lastLocation;
    const char* path;
    std::string newText;
    std::string lastText;
    bool textChanged;
    int startedTweens;
    TextAnimation animation;
    std::vector<Sprite*> sprites;
    Justification just;
};

#endif // __RASTERFONT_H__