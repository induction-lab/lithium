#ifndef __SINGLETON_H__
#define __SINGLETON_H__

template <typename T>
class Singleton {
public:
    static T* getInstance() {
        if (instance == 0) instance = new T();
        return instance;
    };
    static void dispose() {
        if (instance != 0) delete instance;
        instance = 0;
    };
private:
    static T* instance;
protected:
    Singleton() {};
    ~Singleton() {};
};

template <typename T> T* Singleton<T>::instance = 0;

#endif // __SINGLETON_H__