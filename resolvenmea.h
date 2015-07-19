#ifndef RESOLVENMEA_H
#define RESOLVENMEA_H

#include <QObject>
#include <QStringList>
#include <QVector>
#include <QMap>

class ReSolveNMEA : public QObject
{
    Q_OBJECT
public:
    explicit ReSolveNMEA(QObject *parent = 0);

signals:
    void dateUpdate(QStringList latitude,QStringList longitude,
                    int numOfSatellite,QStringList PRNCode,QMap<QString,QVector<QString> > info,
                    float groundSpeed,float zaimuth,int status,float high,QString mode,int style);

public slots:
public:
    void resolve(const QString &string);

private:
    QStringList latitude;//经度
    QStringList longitude;//纬度
    int numOfSatellite;//卫星数量
    QStringList PRNCode;//卫星PRNCode
    QMap<QString,QVector<QString> > info;//每颗卫星的信息
    float groundSpeed;//地面速度
    float zaimuth;//地面航向

    int status;//定位状态
    float high;//海拔高度
    QString mode;//定位模式
    int style;//定位类型

};

#endif // RESOLVENMEA_H
