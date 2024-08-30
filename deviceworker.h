#ifndef DEVICEWORKER_H
#define DEVICEWORKER_H

#include <QObject>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <QDebug>
#include "usb_device.h"
#include "usb2can.h"


class DeviceWorker : public QObject
{
    Q_OBJECT

public:
    explicit DeviceWorker(int id, QObject *parent = nullptr); // 增加一个 id 参数

public slots:
    void process();

private:
    void sendCANFrame();  // 声明发送 CAN 帧的函数
    int workerId;         // 存储线程的 id
    int DevHandle[10];    // 将 DevHandle 声明为成员变量

signals:
    void finished();
};

#endif // DEVICEWORKER_H
