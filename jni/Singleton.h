#ifndef __SINGLETON_H__
#define __SINGLETON_H__

template <typename T>
class Singleton {
private:
    static T* instance;
public:
    static T* getInstance() {
		if (instance == 0) {
			instance = new T();
		}
		return instance;
    }
	static void freeInstance() {
		delete instance;
        instance = 0;
	}	
};

template <typename T> T* Singleton<T>::instance = 0;

#endif