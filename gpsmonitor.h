#ifndef GPSMONITOR_H
#define GPSMONITOR_H

#include <QDialog>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <QLabel>
#include <QTextBrowser>
#include <QLineEdit>
#include <QFile>
#include "detectedserialportthread.h"
#include "receivedatathread.h"
#include "resolvenmea.h"
#include "steerwheelwidget.h"
#include "myspeedwatch.h"
#include "myindicator.h"
#include "gpschart.h"
#include "qcustomplot.h"
#define NUMBER_OF_SATELLITE 12 //卫星数量
#define COLUMN_NUM 6    //每行绘制6个卫星PRN图

namespace Ui {
class GPSMonitor;
}

enum State {Opened,Closed};//表示当前串口的开闭状态

//class GPSMonitor : public QDialog
class GPSMonitor : public QDialog
{
    Q_OBJECT

public:
    explicit GPSMonitor(QWidget *parent = 0);
    ~GPSMonitor();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_btnMenu_Close_clicked();

    void on_btnMenu_Max_clicked();

    void on_btnMenu_Min_clicked();

    //响应串口检测线程发送过来的串口发生变化的信号
    void onSerialPortChanged(bool state,QString name);

    void onSerialPortError(QString);
    void onSerialPortRequeat(QString data);
    void onSerialportSuccess();

    void onOpenSerialPort();//打开串口 按钮响应事件

    void onDataUpdate(QStringList latitude,QStringList longitude,
                      int numOfSatellite,QStringList PRNCode,QMap<QString,QVector<QString> > info,
                      float groundSpeed,float zaimuth,int status,float high,QString mode,int style);
    void onOpenFile();

private:
    Ui::GPSMonitor *ui;

    QPoint mousePoint;
    bool mousePressed;
    bool max;
    QRect location;
    DetectedSerialPortThread *thread;//检测串口状态线程

    ReceiveDataThread receiveDataThread;//接收数据线程

    //各种控件
    QComboBox *portNameCombo;//端口
    QComboBox *baudRateCombo;//波特率
    QComboBox *parityCombo;//校验位
    QComboBox *dataBitsCombo;//数据位
    QComboBox *stopBitsCombo;//停止位
    QPushButton *openSerialPortButton;//打开串口

    State state;//串口当前状态

    ReSolveNMEA resolveNMEA;//解析数据类

    mySpeedWatch *speedWatch;//速度表盘
    SteerWheelWidget *steerWheelWidget;//方向盘

    GPSChart *gpsChart;//星位图

    myIndicator* indicator[NUMBER_OF_SATELLITE];

    QLabel *satelliteNumLabel;//卫星数量标签
    QLabel *statusLabel;//定位状态标签
    QLabel *modeLabel;//模式标签
    QLabel *styleLabel;//类型标签
    QLabel *highLabel;//海拔标签
    QLabel *latitudeLabel;//纬度标签
    QLabel *longitudeLabel;//经度标签

    QPushButton *openFileButton;//选择文件按钮

    QTextBrowser *dataBrowser;//文本浏览器

    QCustomPlot *highErrorPlot;//高度误差曲线
    QCustomPlot *longitudeErrorPlot;//经度纬度误差曲线
    QCustomPlot *latitudeErrorPlot;//纬度误差曲线
    QCustomPlot *dotPlot;//打点图
    QCustomPlot *totalDiffPlot;//总的误差曲线

    QLineEdit *highEdit;//标准高度输入框
    QLineEdit *longitudeEdit;//标准经度输入框
    QLineEdit *latitudeEdit;//标准纬度输入框

    QVector<double> highDiff;//高度误差
    QVector<double> longitudeDiff;//精度误差
    QVector<double> latitudeDiff;//纬度误差
    QVector<double> totalDiff;//总的误差

    QVector<double> x;//高度，经度，纬度，总的 误差的横坐标
    int startX;//横坐标的起始值

    QVector<double> dotX;//打点图横坐标
    QVector<double> dotY;//打点图纵坐标

    QFile outputFile;//把接收到的数据保存到文件中


private:
    void InitStyle();

    void initComboBox();//初始化ComboBox控件

    void  resetPortNameCombo();//重置portNameCombo内容

    void enabledCombo(bool enabled);//combo控件使能/禁能

    QString getItude(const QStringList &strData);//从data中获得经纬度的

    void initPlot();//初始化曲线图

    double str2double(const QString &str);//把字符表示的数据转化为十进制的数字表示的经纬度
    //把小数表示的经纬度数据转换为以度(°)为单位的数据
    double double2du(const double data);

};

#endif // GPSMONITOR_H
