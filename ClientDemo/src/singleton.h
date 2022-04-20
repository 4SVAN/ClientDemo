#ifndef SINGLETON_H
#define SINGLETON_H

#include <QMutex>
#include <QScopedPointer>

template <typename T>
class Singleton
{
public:
    static T &getInstance(); // 获取单例的唯一实例对象

    Singleton(const Singleton &other) = delete;
    Singleton<T> &operator=(const Singleton &other) = delete;

private:
    static QMutex mutex;
    static QScopedPointer<T> instance;
};

/*-----------------------------------------------------------------------------|
 |                          Singleton Implementation                           |
 |----------------------------------------------------------------------------*/
template <typename T> QMutex Singleton<T>::mutex;
template <typename T> QScopedPointer<T> Singleton<T>::instance;

template <typename T>
T &Singleton<T>::getInstance()
{
    if (instance.isNull()) {
        mutex.lock();
        if (instance.isNull()) {
            instance.reset(new T()); // 此指针会在全局变量作用域结束时自动 deleted (main 函数返回后)
        }
        mutex.unlock();
    }

    return *instance.data();
}

/*-----------------------------------------------------------------------------|
 |                               Singleton Macro                               |
 |----------------------------------------------------------------------------*/
#define SINGLETON(Class)                           \
private:                                           \
    Class() = default;                             \
    ~Class() = default;                            \
    Class(const Class &other) = delete;            \
    Class& operator=(const Class &other) = delete; \
    friend class  Singleton<Class>;                \
    friend struct QScopedPointerDeleter<Class>;    \
                                                   \
public:                                            \
    static Class& getInstance() {                  \
        return Singleton<Class>::getInstance();    \
    }

#endif // SINGLETON_H
