#include "DeviceWorker.h"

#define GET_FIRMWARE_INFO   1
#define CAN_MODE_LOOPBACK   1
#define CAN_SEND_MSG        1
#define CAN_GET_MSG         1
#define CAN_GET_STATUS      1

DeviceWorker::DeviceWorker(int id, QObject *parent) :
    QObject(parent), workerId(id)
{
    qDebug()<<"线程id:"<<workerId;
    int numDevices = USB_ScanDevice(DevHandle);  // 获取设备数量

    if (numDevices > 0) {
        for (int i = 0; i < numDevices; ++i) {
            qDebug() << "Device ID:" << DevHandle[i];  // 打印每个设备的设备号
        }
    } else {
        qDebug() << "No devices found!";
    }

}

void DeviceWorker::process()
{

    int CANIndex = 0; // USB2XXX 只支持一个通道，所以该参数必须为0
    bool state;
    int ret;

    // 扫描查找设备
    ret = USB_ScanDevice(DevHandle);
    if (ret <= 0) {
        qDebug() << QString().sprintf("未连接设备!");
        emit finished();
        return;
    } else {
        qDebug() << QString().sprintf("设备连接成功!");
    }

    // 打开设备
    state = USB_OpenDevice(DevHandle[0]);
    if (!state) {
        qDebug() << QString().sprintf("打开设备失败!");
        emit finished();
        return;
    } else {
        qDebug() << QString().sprintf("设备打开成功!");
    }

    // 获取固件信息（可选）
    #if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
    char FunctionStr[256] = {0};
    state = DEV_GetDeviceInfo(DevHandle[0], &DevInfo, FunctionStr);
    if (!state) {
        qDebug() << QString().sprintf("获取设备信息失败!");
        emit finished();
        return;
    } else {
        qDebug() << QString().sprintf("固件信息:");
        qDebug() << QString().sprintf("固件名称: %s", DevInfo.FirmwareName);
        qDebug() << QString().sprintf("固件编译日期: %s", DevInfo.BuildDate);
        qDebug() << QString().sprintf("固件版本: v%d.%d.%d",
            (DevInfo.FirmwareVersion >> 24) & 0xFF,
            (DevInfo.FirmwareVersion >> 16) & 0xFF,
            DevInfo.FirmwareVersion & 0xFFFF);
        qDebug() << QString().sprintf("硬件版本: v%d.%d.%d",
            (DevInfo.HardwareVersion >> 24) & 0xFF,
            (DevInfo.HardwareVersion >> 16) & 0xFF,
            DevInfo.HardwareVersion & 0xFFFF);
        qDebug() << QString().sprintf("固件功能: %s", FunctionStr);
    }
    #endif

    // 初始化配置CAN
    CAN_INIT_CONFIG CANConfig;
    #if CAN_MODE_LOOPBACK
    CANConfig.CAN_Mode = 1; // 环回模式
    #else
    CANConfig.CAN_Mode = 0; // 正常模式
    #endif
    CANConfig.CAN_ABOM = 0; // 禁止自动离线
    CANConfig.CAN_NART = 1; // 禁止报文重传
    CANConfig.CAN_RFLM = 0; // FIFO满之后覆盖旧报文
    CANConfig.CAN_TXFP = 1; // 发送请求决定发送顺序
    CANConfig.CAN_BRP = 4;
    CANConfig.CAN_BS1 = 16;
    CANConfig.CAN_BS2 = 4;
    CANConfig.CAN_SJW = 1;

    // 配置 CAN
    ret = CAN_Init(DevHandle[0], CANIndex, &CANConfig);
    if (ret != CAN_SUCCESS) {
        qDebug() << QString().sprintf("CAN 配置失败!");
        qDebug() <<"DevHandle[0]"<<DevHandle[0]<<"CANIndex"<<CANIndex;
        emit finished();
        return;
    } else {
        qDebug() << QString().sprintf("CAN 配置成功!");
    }

    // 发送 CAN 帧
    sendCANFrame();

    // 关闭设备
    USB_CloseDevice(DevHandle[0]);
    emit finished();
}

void DeviceWorker::sendCANFrame()
{
    // 定义 CAN 帧
    CAN_MSG CanMsg;
    CanMsg.ExternFlag = 0;  // 标准帧
    CanMsg.RemoteFlag = 0;  // 数据帧
    CanMsg.ID = 0x100 + workerId;  // 根据线程 ID 生成不同的帧 ID
    CanMsg.DataLen = 8;     // 数据长度为 8

    // 根据线程 ID 生成不同的帧数据
    for (int i = 0; i < CanMsg.DataLen; i++) {
        CanMsg.Data[i] = workerId + i; // 示例数据，依据线程 ID 变化
    }

    // 发送 CAN 帧
    int ret = CAN_SendMsg(DevHandle[0], 0, &CanMsg, 1);
    if (ret >= 0) {
        qDebug() << QString().sprintf("线程 %d 成功发送 CAN 帧: ID = 0x%X", workerId, CanMsg.ID);
    } else {
        qDebug() << QString().sprintf("线程 %d 发送 CAN 帧失败!", workerId);
    }
}

