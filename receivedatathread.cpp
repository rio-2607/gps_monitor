#include "receivedatathread.h"
#include <QTime>
#include <QDebug>

QT_USE_NAMESPACE

ReceiveDataThread::ReceiveDataThread(QObject *parent)
    : QThread(parent),quit(false)
{
}

ReceiveDataThread::~ReceiveDataThread()
{
    mutex.lock();
    quit = true;
    mutex.unlock();
    wait();
}

void ReceiveDataThread::startReceiveData(const QString &portName, int baudRate,
                                         QSerialPort::Parity parity, QSerialPort::StopBits stopBits,
                                         QSerialPort::DataBits dataBits)
{
    QMutexLocker locker(&mutex);
    this->portName = portName;
    this->baudRate = baudRate;
    this->parity = parity;
    this->stopBits = stopBits;
    this->dataBits = dataBits;

    if (!isRunning())
        start();
}

void ReceiveDataThread::run()
{
    bool currentPortNameChanged = false;

    quit = false;//这一句是必须的

    mutex.lock();
    QString currentPortName;
    if (currentPortName != portName)
    {
        currentPortName = portName;
        currentPortNameChanged = true;
    }
    mutex.unlock();
    QSerialPort serial;

    while (!quit)
    {
        if (currentPortNameChanged)
        {
            serial.close();
            serial.setPortName(currentPortName);
            serial.setBaudRate(baudRate);
            serial.setStopBits(stopBits);
            serial.setParity(parity);
            serial.setDataBits(dataBits);

            if (!serial.open(QIODevice::ReadWrite))
            {
                emit error(tr("无法打开串口%1, 错误码为%2")
                           .arg(portName).arg(serial.error()));
                return;
            }
            //成功打开串口
            emit success();
        }

        if (serial.waitForReadyRead(100))
        {
            //读取数据
//            QByteArray requestData = serial.readAll();
            data += serial.readAll();
//            while (serial.waitForReadyRead(1000))
//                requestData += serial.readAll();

//            qDebug() << "requestData.size() = " << requestData.size();

            //如果接收到的数据加起来超过了800个字节,则发送给主界面
//           if(requestData.size() > 800)
            if(data.size() > 400)
           {
//               QString request(requestData);
               qDebug() << "data.size() = " << data.size();
               QString request(data);
               emit this->request(request);//发送数据
               data.clear();
           }
        }
        mutex.lock();
        if (currentPortName != portName)
        {
            currentPortName = portName;
            currentPortNameChanged = true;
        }
        else
        {
            currentPortNameChanged = false;
        }
        mutex.unlock();
    }
    serial.close();

    qDebug() << "串口被关闭" << endl;

}

void ReceiveDataThread::setQuitFlag(bool flag)
{
    this->quit = flag;
}
