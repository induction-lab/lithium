#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <android_native_app_glue.h>
#include <list>

#include "EventLoop.h"

class Kernel {
private:
    typedef std::list<Task*>			TaskList;
    typedef std::list<Task*>::iterator	TaskListIterator;
    TaskList	m_tasks;
    TaskList	m_pausedTasks;
    void PriorityAdd(Task* pTask) {
        TaskListIterator iter;
        for (iter = m_tasks.begin(); iter != m_tasks.end(); ++iter) {
            Task* pCurrentTask = (*iter);
            if (pCurrentTask->Priority() > pTask->Priority()) break;
        }
        m_tasks.insert(iter, pTask);
    }
public:
    Kernel() {}
    ~Kernel() {}
    void Execute() {
		LOG_INFO("Starting Kernel"); 
        while (m_tasks.size()) {
            if (EventLoop::IsClosing()) KillAllTasks();
            TaskListIterator iter;
            for (iter = m_tasks.begin(); iter != m_tasks.end(); ++iter) {
                Task* pTask = (*iter);
                if (!pTask->CanKill()) pTask->Update();
            }
            for (iter = m_tasks.begin(); iter != m_tasks.end();) {
                Task* pTask = (*iter);
                ++iter;
                if (pTask->CanKill()) {
                    pTask->Stop();
                    m_tasks.remove(pTask);
                    pTask = 0;
                }
            }
        }
        EventLoop::ClearClosing();
    }
    bool AddTask(Task* pTask) {
        bool started = pTask->Start();
        if (started) PriorityAdd(pTask);
        return started;
    }
    void SuspendTask(Task* pTask) {
        // Ensure that task is running
        if (std::find(m_tasks.begin(), m_tasks.end(), pTask) != m_tasks.end()) {
            pTask->OnSuspend();
            m_tasks.remove(pTask);
            m_pausedTasks.push_back(pTask);
        }
    }
    void ResumeTask(Task* pTask) {
        if (std::find(m_pausedTasks.begin(), m_pausedTasks.end(), pTask) != m_pausedTasks.end()) {
            pTask->OnResume();
            m_pausedTasks.remove(pTask);
            PriorityAdd(pTask);
        }
    }
    void RemoveTask(Task* pTask) {
        if (std::find(m_tasks.begin(), m_tasks.end(), pTask) != m_tasks.end()) pTask->SetCanKill(true);
    }
    void KillAllTasks() {
        for (TaskListIterator iter = m_tasks.begin(); iter != m_tasks.end(); ++iter) {
            (*iter)->SetCanKill(true);
        }
    }
};

#endif
