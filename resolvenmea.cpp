#include "resolvenmea.h"
#include <QStringList>
#include <QString>
#define SIGIFICANCE_BIT 6
#include <QDebug>

ReSolveNMEA::ReSolveNMEA(QObject *parent) :
    QObject(parent)
{
}

void ReSolveNMEA::resolve(const QString &string)
{
    bool isResolving = false;//是否已经开始解析数据
    QStringList sourceList = string.split("$");//分离出每个单独的句子
    foreach (QString s, sourceList)
    {
        QString s1 = s.trimmed();//去掉每个句子末尾的空白字符
        if(!s1.isEmpty())
        {
            //判断是否是一个完整的句子,只需要判断前面
            //两个字母是不是GP，以及倒数第三个字符是不是*号
            if("GP" == s1.left(2) && "*" == s1.at(s1.length() - 3))
            {
                QStringList codeList = s1.split(",");
                if("GPGGA" == codeList.at(0) &&
                        "1" == codeList.at(SIGIFICANCE_BIT))//如果是GPGGA且定位有效
                {
                    //则开始解码
                    isResolving = true;
                    //每次开始解码都要把数据清空
                    latitude.clear();
                    longitude.clear();
                    PRNCode.clear();
                    info.clear();
                    latitude << codeList.at(2) << codeList.at(3);
                    longitude << codeList.at(4) << codeList.at(5);
                    numOfSatellite = codeList.at(7).toInt();
                    status = codeList.at(6).toInt();
                    high = codeList.at(9).toFloat();
                }

                if(isResolving)//如果已经开始解码
                {
                    if("GPGSA" == codeList.at(0))//解析GPGSA
                    {
                        for(int i = 3;i <= 14;++i)
                            PRNCode.append(codeList.at(i));
                        mode = codeList.at(1);
                        style = codeList.at(2).toInt();
                    }
                    if("GPGSV" == codeList.at(0))//如果是GPGSV
                    {
                        QVector<QString> vec;
                        int totalNumOfSentence = codeList.at(1).toInt();//总的语句数
                        int currentNumOfSentence = codeList.at(2).toInt();//当前的语句数
                        int totalNumOfSatellite = codeList.at(3).toInt();//总的卫星数量
                        if(currentNumOfSentence < totalNumOfSentence)
                        {
                            //如果当前的语句数小于总的语句数，说明当前语句包含四颗卫星的信息
                            for(int i = 4;i < 19;i +=4)
                            {
                                vec.clear();
                                vec.push_back(codeList.at(i+1));
                                vec.push_back(codeList.at(i+2));
                                QString temp = i + 3 == 19 ? codeList.at(i+3).split("*").at(0) : codeList.at(i+3);
                                vec.push_back(temp);
                                info.insert(QString(codeList.at(i)),vec);
                            }
                        }
                        else
                        {
                            //如果当前的语句数等于总的语句数，说明当前语句包含的卫星数量可能小于四颗
                            //当前语句包含的卫星的数量等于总的卫星数量减去前面的语句数乘以4
                            int currentNumOfSatellite = totalNumOfSatellite - (currentNumOfSentence - 1) * 4;
                            int currentNumOfWord = currentNumOfSatellite * 4 + 3;//当前列表包含的成员的个数
                            for(int i = 4;i < currentNumOfWord;i+=4)
                            {
                                vec.clear();
                                vec.push_back(codeList.at(i+1));
                                vec.push_back(codeList.at(i+2));
                                QString temp = i + 3 == currentNumOfWord ? codeList.at(i+3).split("*").at(0) : codeList.at(i+3);
                                vec.push_back(temp);
                                info.insert(QString(codeList.at(i)),vec);
                            }
                        }
                    }
                    if("GPRMC" == codeList.at(0))//如果是GPRMC
                    {
                        groundSpeed = codeList.at(7).toFloat();
                        zaimuth = codeList.at(8).toFloat();
                        isResolving = false;//一帧数据解码结束
                        //每解析完一帧数据，就把解析完的数据发送出去
                        emit dateUpdate(latitude,longitude,numOfSatellite,PRNCode,
                                        info,groundSpeed,zaimuth,
                                        status,high,mode,style);

                    }
                }
            }

        }

    }
}
