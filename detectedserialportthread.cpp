#include "detectedserialportthread.h"

DetectedSerialPortThread::DetectedSerialPortThread(const QList<QSerialPortInfo> &serialPortListInfo)
    : serialPortList(serialPortListInfo)
{
    isSerialPortNumChanged = false;//默认串口数量没有发生变化
}


void DetectedSerialPortThread::run()
{
    QList<QSerialPortInfo> currentSerialPortList;
    QList<QString> formerResult;
    QList<QString> currentResult;
    QString serialPortName;//新增加的或者新拔出的串口的名称
    //serialPortState表示串口的状态，false表示串口被弹出，true表示发现新的串口设备
    bool serialPortState;
    while(true)
    {
        currentSerialPortList = QSerialPortInfo::availablePorts();//获取当前机器可用的串口
        getSerialName(currentSerialPortList,currentResult);
        getSerialName(serialPortList,formerResult);
        if(currentResult.size() < formerResult.size())
        {
            isSerialPortNumChanged = true;//串口数量发生变化
            serialPortState = false;//串口数量减少，有串口被拔出
            QList<QString>::const_iterator iter = formerResult.begin();
            while(iter != formerResult.end())
            {
                if(!currentResult.contains(*iter))
                {
                    serialPortName = *iter;
                    break;
                }
                else
                    ++iter;
            }
        }
        else if(currentResult.size() > formerResult.size())
        {
            isSerialPortNumChanged = true;//串口数量发生了变化
            serialPortState = true;//串口数量增加，新插入了串口
            QList<QString>::const_iterator iter = currentResult.begin();
            while(iter != currentResult.end())
            {
                if(!formerResult.contains(*iter))
                {
                    serialPortName = *iter;
                    break;
                }
                else
                    ++iter;
            }
        }

        if(isSerialPortNumChanged)
        {
            serialPortList = currentSerialPortList;
            emit serialPortChanged(serialPortState,serialPortName);

            //每次发送完信号之后都要把isSerialPortNumChanged设置为false
            isSerialPortNumChanged = false;
        }
    }

//    exec();
}


//从info中获取每个可用串口的名字，保存在result中
void DetectedSerialPortThread::getSerialName(const QList<QSerialPortInfo> &info,
                                             QList<QString> &result)
{
    //注意，下面把 result清空是必须的，否则会进入无限循环当中
    if(!result.isEmpty())
        result.clear();
    QList<QSerialPortInfo>::const_iterator iter = info.begin();
    while(iter != info.end())
    {
        result.push_back(iter->portName());
        ++iter;
    }
}
