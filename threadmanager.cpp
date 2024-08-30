#include "threadmanager.h"

ThreadManager::ThreadManager(QObject *parent) : QObject(parent)
{

}

void ThreadManager::addWorker(int id)
{
    // 创建新的线程和工作对象
    QThread *thread = new QThread;
    DeviceWorker *worker = new DeviceWorker(id);

    // 将工作对象移入线程
    worker->moveToThread(thread);

    // 连接开始信号和工作对象的处理槽
    connect(thread, &QThread::started, worker, &DeviceWorker::process);
    // 连接工作对象的完成信号到线程的停止槽
    connect(worker, &DeviceWorker::finished, thread, &QThread::quit);
    // 连接工作对象的完成信号和线程停止槽
    connect(worker, &DeviceWorker::finished, this, &ThreadManager::handleWorkerFinished);

    // 将线程和工作对象添加到队列中
    threadQueue.enqueue(thread);
    workerQueue.enqueue(worker);
}

void ThreadManager::startNextThread()
{
    if (!threadQueue.isEmpty() && !workerQueue.isEmpty()) {
        // 取出队列中的下一个线程和工作对象
        QThread *thread = threadQueue.dequeue();
        DeviceWorker *worker = workerQueue.dequeue();

        // 连接线程的完成信号到管理器的槽以删除对象
        connect(thread, &QThread::finished, worker, &DeviceWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        // 启动线程
        thread->start();
    } else {
        qDebug() << "All threads have finished executing.";
    }
}

void ThreadManager::handleWorkerFinished() {
    qDebug() << "Worker finished, starting next thread if available.";
    startNextThread();  // 继续处理下一个线程
}

ThreadManager::~ThreadManager()
{

}
