#ifndef DETECTEDSERIALPORTTHREAD_H
#define DETECTEDSERIALPORTTHREAD_H

/**
  ****时间:2014/11/3/19:44
  ****功能:专有线程，每隔一秒检查串口状态
  ****检查并发下新的串口
  */
#include <QThread>
#include <QSerialPortInfo>
#include <QList>
#include <QString>

class DetectedSerialPortThread : public QThread
{
        Q_OBJECT
public:
    DetectedSerialPortThread(const QList<QSerialPortInfo> &serialPortListInfo);
    DetectedSerialPortThread(){}
protected:
    void run();
signals:
    //当串口被插入或者拔出时发送这个信号
    //state表示串口是被插入还是拔出，那么表示发生变化的串口的名字
    void serialPortChanged(bool state,QString name);
private:
    QList<QSerialPortInfo> serialPortList;
    bool isSerialPortNumChanged;//标志串口数量是否发生变化
private:
    //从info中获取每个可用串口的名字，保存在result中
    void getSerialName(const QList<QSerialPortInfo> &info,QList<QString> &result);
};

#endif // DETECTEDSERIALPORTTHREAD_H
