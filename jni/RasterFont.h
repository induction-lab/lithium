#ifndef __RASTERFONT_H__
#define __RASTERFONT_H__

#include "SpriteBatch.h"

// For text justification
enum class Justification {
    LEFT, MIDDLE, RIGHT
};

// Text animation.
enum class TextAnimation {
    NONE, SCALE, SLIDE
};

class RasterFont {
public:
    const int CHAR_PADDING = 44;
    RasterFont(const char* path, int width, int height, Vector2 location, Justification just, TextAnimation animation = TextAnimation::NONE):
        path(path), width(width), height(height), location(location),
        newText("\0"),
        lastText("\0"),
        textChanged(false),
        animated(0),
        animation(animation),
        scale(Vector2(1.0f, 1.0f)),
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
        if (newText.compare(lastText) == 0 && !textChanged) return;
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
            Vector2 l = Vector2(location.x + (width - CHAR_PADDING) * count * scale.x, location.y + frand(4) - 2.0f);
            if (just == Justification::MIDDLE) {
                l.x = l.x - textWidth / 2 * scale.x;
            } else if (just == Justification::RIGHT) {
                l.x = l.x - textWidth * scale.x;
            }
            // Create new sprite.
            if (q || (count >= lastTextLength)) {
                Sprite* sprite = spriteBatch->registerSprite(path, width, height);
                sprite->setFrame(text[count]);
                sprite->location = l;
                sprite->scale = scale;
                if (animation == TextAnimation::SLIDE) sprite->opaque = 0.0f;
                // Scale animation.
                if (animation == TextAnimation::SCALE) {
                    Tween* t1 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_XY, 0.1f, Ease::Sinusoidal::InOut)
                        ->target(1.5f, 1.5f)->remove(true);
                    Tween* t2 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_XY, 0.25f, Ease::Sinusoidal::InOut)
                        ->target(1.0f, 1.0f)->remove(true)->onComplete(std::bind(&RasterFont::onAnimatedComplete, this));
                    animated++;
                    t1->addChain(t2)->start();
                }
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
        // Slide animation.
        if (animation == TextAnimation::SLIDE) {
            for (std::vector<Sprite*>::iterator it = sprites.begin(); it < sprites.end(); ++it) {
                lastLocation = (*it)->location;
                Tween* t1 = TweenManager::getInstance()->addTween(*it, TweenType::OPAQUE, 0.25f, Ease::Sinusoidal::InOut)
                    ->target(1.0f)->remove(true);                
                Tween* t2 = TweenManager::getInstance()->addTween(*it, TweenType::POSITION_Y, 0.25f, Ease::Back::Out)
                    ->target(lastLocation.y + 20.0f)->remove(true)->start();
                Tween* t3 = TweenManager::getInstance()->addTween(*it, TweenType::OPAQUE, 0.25f, Ease::Sinusoidal::InOut)
                    ->target(0.0f)->remove(true)->delay(1.0f)->delay(0.7f);
                Tween* t4 = TweenManager::getInstance()->addTween(*it, TweenType::POSITION_Y, 0.25f, Ease::Sinusoidal::InOut)
                    ->target(lastLocation.y)->remove(true)->delay(0.7f)->onComplete(std::bind(&RasterFont::onAnimatedComplete, this));
                animated++;
                t1->addChain(t3);
                t3->addChain(t4);
                t1->start();
                textChanged = false;
            }
        }
        lastText = text;
    }
    void onAnimatedComplete() {
        animated--;
    }
    void setText(const char* text) {
        newText = text;
        textChanged = true;
    };
    Vector2 scale;
private:
    int width, height;
    Vector2 location;
    Vector2 lastLocation;
    SpriteBatch* spriteBatch;
    const char* path;
    std::string newText;
    std::string lastText;
    bool textChanged;
    int animated;
    TextAnimation animation;
    std::vector<Sprite*> sprites;
    Justification just;
};

#endif // __RASTERFONT_H__