#ifndef __TIMER_H__
#define __TIMER_H__

#include "Kernel.h"

class Timer: public Task {
public:
    typedef long long TimeUnits;
private:
    TimeUnits nanoTime() {
        timespec now;
        int err = clock_gettime(CLOCK_MONOTONIC, &now);
        return now.tv_sec*1000000000LL + now.tv_nsec;
    }
    TimeUnits	m_timeLastFrame;
    float		m_frameDt;
    float		m_simDt;
    float		m_simMultiplier;
public:
    Timer(const unsigned int priority):
		Task(priority),
        m_timeLastFrame(0),
        m_frameDt(0.0f),
        m_simDt(0.0f),
        m_simMultiplier(1.0f) {
    }
    ~Timer() {}
    bool Start() {
        m_timeLastFrame = nanoTime();
        return true;
    }
    void OnSuspend() {}
    void Update() {
        // Get the delta between the last frame and this
        TimeUnits currentTime = nanoTime();
        const float MULTIPLIER = 0.000000001f;
        m_frameDt = (currentTime-m_timeLastFrame) * MULTIPLIER;
        m_timeLastFrame = currentTime;
        if (m_frameDt > 0.2f) m_frameDt = 0.2f;
        m_simDt = m_frameDt * m_simMultiplier;
    }
    void OnResume() {
        m_timeLastFrame = nanoTime();
    }
    void Stop() {}
    float GetTimeFrame() const {
        return m_frameDt;
    }
    float GetTimeSim() const {
        return m_simDt;
    }
    void SetSimMultiplier(const float simMultiplier) {
        m_simMultiplier = simMultiplier;
    }
};

#endif
