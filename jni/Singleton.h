#ifndef __SINGLETON_H__
#define __SINGLETON_H__

template <typename T>
class Singleton {
private:
    static T* instance;
public:
    Singleton() {
        instance = static_cast<T*>(this);
    }
    ~Singleton() {
        instance = 0;
    }
    static T& getInstance()		{
        return *instance;
    }
    static T* getInstancePtr()	{
        return instance;
    }
};

template <typename T> T* Singleton<T>::instance = 0;

#endif