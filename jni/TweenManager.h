#ifndef __TWEENMANAGER_H__
#define __TWEENMANAGER_H__

#include <list>

#include "Singleton.h"
#include "TimeManager.h"
#include "Tween.h"

class TweenType {
public:
    static const int POSITION_X = 0x01;
    static const int POSITION_Y = 0x02;
    static const int POSITION_XY = 0x04;
    static const int SCALE_X = 0x08;
    static const int SCALE_Y = 0x10;
    static const int SCALE_XY = 0x20;
    static const int ROTATION_CW = 0x40;
    static const int ROTATION_CCW = 0x80;
    static const int OPAQUE = 0x0F;
    static const int COLOR = 0xF0;
    static const int FRAME = 0xF2;
};

class TweenManager: public Singleton<TweenManager> {
public:
    TweenManager():
        started(false) {
        LOG_INFO("Creating TweenManager.");
    };
    ~TweenManager() {
        LOG_INFO("Destructing TweenManager.");
        reset();
    };
    Tween* makeTween(Tweenable* target, int tweenType = -1, float duration = 1000, EaseFunc ease=Ease::Linear) {
        return new Tween(target, tweenType, duration, ease);
    };
    Tween* addTween(Tweenable* target, int tweenType, float duration, EaseFunc ease) {
        Tween* t = makeTween(target, tweenType, duration, ease);
        return addTween(t);
    };
    Tween* addTween(Tween* t) {
        tweens.push_back(t);
        return t;
    };
    status start() {
        LOG_INFO("Starting TweenManager.");
        started = true;
        return STATUS_OK;
    };
    void stop() {
        LOG_INFO("Stopping TweenManager.");
        started = false;
    };
    status update() {
        if (started) {
            for (std::list<Tween*>::reverse_iterator it = tweens.rbegin(); it != tweens.rend(); ++it) {
                // Update the tween.
                (*it)->update(TimeManager::getInstance()->getTime());
                // Remove stopped tweens.
                if((*it)->getCompleted() && (*it)->getAutoRemove()) {
                    SAFE_DELETE(*it);
                    tweens.remove(*it);
                }
            }
        }
        return STATUS_OK;
    };
    void remove(Tween* t) {
        std::list<Tween*>::iterator it = find(tweens.begin(), tweens.end(), t);
        if (it != tweens.end()) {
            SAFE_DELETE(*it);            
            tweens.erase(it);
        }
    };
    void reset() {
        LOG_DEBUG("Delete %d tweens.", tweens.size());
        for (std::list<Tween*>::reverse_iterator it = tweens.rbegin(); it != tweens.rend(); ++it) {
            SAFE_DELETE(*it);
        }
        tweens.clear();
    };
    // Debug.
    int getTweensCount() {
        return tweens.size();
    };
private:
    bool started;
    std::list<Tween*> tweens;
};

#endif // __TWEENMANAGER_H__