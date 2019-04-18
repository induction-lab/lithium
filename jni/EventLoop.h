#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

class Task {
private:
    unsigned int	m_priority;
    bool			m_canKill;
public:
    Task(const unsigned int priority) {
        m_priority = priority;
        m_canKill = false;
    }
    ~Task() {}
    virtual bool	Start()		= 0;
    virtual void	OnSuspend()	= 0;
    virtual void	Update()	= 0;
    virtual void	OnResume()	= 0;
    virtual void	Stop()		= 0;
    static const unsigned int	TIMER_PRIORITY		= 0;
    static const unsigned int	PLATFORM_PRIORITY	= 1000;
    static const unsigned int	FILE_PRIORITY		= 2000;
    static const unsigned int	GAME_PRIORITY		= 3000;
    static const unsigned int	AUDIO_PRIORITY		= 4000;
    static const unsigned int	RENDER_PRIORITY		= 5000;
    void SetCanKill(const bool canKill) {
        m_canKill = canKill;
    }
    bool CanKill() const {
        return m_canKill;
    }
    unsigned int Priority() const {
        return m_priority;
    }
};

#include "Renderer.h"

class EventLoop: public Task {
private:
    static bool		m_bClosing;
    static bool		m_bPaused;
    android_app*	m_pState;
public:
    EventLoop(android_app* pState, unsigned int priority, Renderer* pRenderer): Task(priority) {
        m_pState = pState;
        m_pState->onAppCmd = android_handle_cmd;
		m_pState->userData = static_cast<void*>(pRenderer);
		LOG_INFO("Creating EventLoop");
    }
    ~EventLoop() {
		LOG_INFO("Destructing EventLoop");		
	}
    bool Start() {
        return true;
    }
    void OnSuspend() {
    }
    void Update() {
        int events;
        struct android_poll_source* pSource;
        int ident = ALooper_pollAll(0, 0, &events, (void**)&pSource);
        if (ident >= 0) {
            if (pSource) pSource->process(m_pState, pSource);
            if (m_pState->destroyRequested) m_bClosing = true;
        }
    }
    void OnResume() {}
    void Stop() {}
    static void ClearClosing() {
        m_bClosing = false;
    }
    static bool IsClosing()	{
		return m_bClosing;
	}
    static void SetPaused(const bool paused) {
        m_bPaused = paused;
    }
    static bool IsPaused() {
        return m_bPaused;
    }
    static void android_handle_cmd(struct android_app* app, int32_t cmd) {
        switch (cmd) {
		case APP_CMD_INIT_WINDOW: {
			Renderer* pRenderer = static_cast<Renderer*>(app->userData);
			assert(pRenderer);
			pRenderer->Init();
		}
		break;
		case APP_CMD_DESTROY: {
			Renderer* pRenderer = static_cast<Renderer*>(app->userData);
			assert(pRenderer);
			pRenderer->Destroy();
		}
		break;
		case APP_CMD_TERM_WINDOW: {
			Renderer* pRenderer = static_cast<Renderer*>(app->userData);
			assert(pRenderer);
			pRenderer->Destroy();
		}
		break;			
        case APP_CMD_RESUME: {
            SetPaused(false);
        }
        break;
        case APP_CMD_PAUSE: {
            SetPaused(true);
        }
        break;
        }
    }
};

bool EventLoop::m_bClosing	= false;
bool EventLoop::m_bPaused		= false;

#endif
