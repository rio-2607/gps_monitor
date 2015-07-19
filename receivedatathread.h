#ifndef SLAVETHREAD_H
#define SLAVETHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSerialPort>
#include <QByteArray>

class ReceiveDataThread : public QThread
{
    Q_OBJECT

public:
    ReceiveDataThread(QObject *parent = 0);
    ~ReceiveDataThread();

    void startReceiveData(const QString &portName, int baudRate, QSerialPort::Parity parity,
                          QSerialPort::StopBits stopBits, QSerialPort::DataBits dataBits);
    void run();

    void setQuitFlag(bool flag);//设置quit标志位

signals:
    void request(const QString &s);//收集到了数据
    void error(const QString &s);//串口出现错误
    void success();//串口成功打开

private:
    QString portName;
//    QString response;
//    int waitTimeout;
    QMutex mutex;
    bool quit;

    int baudRate;
    QSerialPort::StopBits stopBits;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;

    QByteArray data;
};

#endif // SLAVETHREAD_H
