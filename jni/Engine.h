#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Kernel.h"
#include "Timer.h"

class Engine {
private:
    Kernel		m_kernel;
    EventLoop	m_eventLoop;
    Timer		m_timer;
	Renderer 	m_renderer;
public:
    explicit Engine(android_app* state):
		m_renderer(state, Task::RENDER_PRIORITY),
		m_eventLoop(state, Task::PLATFORM_PRIORITY, &m_renderer),
        m_timer(Task::TIMER_PRIORITY) {
			LOG_INFO("Creating Engine");
			//File::SetAssetManager(state->activity->assetManager);
    }
    ~Engine() {
		LOG_INFO("Destructing Engine");
	}
    bool Initialize() {
        bool ret = true;
        m_kernel.AddTask(&m_eventLoop);
        m_kernel.AddTask(&m_timer);
		m_kernel.AddTask(&m_renderer);
        return ret;
    }
    void Run() {
        m_kernel.Execute();
    }
};

#endif
