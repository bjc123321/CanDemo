#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QObject>
#include <QQueue>
#include <QThread>
#include <QDebug>
#include "DeviceWorker.h"

class ThreadManager : public QObject
{
    Q_OBJECT
public:

    static ThreadManager& getInstance() {
        static ThreadManager instance; // 静态实例对象
        return instance;
    }

    void addWorker(int id);
    void startNextThread();

private slots:
    void handleWorkerFinished();


private:

    ThreadManager(QObject *parent = nullptr);  // 私有化构造函数
    ~ThreadManager();

    ThreadManager(const ThreadManager&) = delete;  // 禁止拷贝构造
    ThreadManager& operator=(const ThreadManager&) = delete;  // 禁止赋值操作

    QQueue<QThread*> threadQueue;  // 存储线程的队列
    QQueue<DeviceWorker*> workerQueue;  // 存储工作对象的队列

};

#endif // THREADMANAGER_H
