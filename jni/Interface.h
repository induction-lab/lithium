#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <functional>
#include "SoundManager.h"
#include "GraphicsManager.h"
#include "SpriteBatch.h"

class Widget: public InputListener {
public:
    Widget(int32_t width, int32_t height, Location location):
        width(width),
        height(height),
        location(location),
        downFunction(NULL),
        upFunction(NULL),
        clickFunction(NULL) {
        //
    };
    virtual void update() {};
    bool pointInWidget(Location point) {
        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;
        return ((point.x > location.x - halfWidth ) &&
                (point.x < location.x + halfWidth ) &&
                (point.y > location.y - halfHeight) &&
                (point.y < location.y + halfHeight));
    };
    void setDownFunction(std::function<void()> callback) {
        downFunction = callback;
    };
    void setUpFunction(std::function<void()> callback) {
        upFunction = callback;
    };
    void setClickFunction(std::function<void()> callback) {
        clickFunction = callback;
    };
protected:
    friend class Interface;
    int32_t width, height;
    Location location;
    std::function<void()> downFunction;
    std::function<void()> upFunction;
    std::function<void()> clickFunction;
};

#endif // __INTERFACE_H__