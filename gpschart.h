#ifndef GPSCHART_H
#define GPSCHART_H

#include <QWidget>
#include <QMap>
#include <QVector>
#include <QString>
#include <QMouseEvent>
#include <QRectF>
#include <QPixmap>
#include <QStringList>

class GPSChart : public QWidget
{
    Q_OBJECT

public:
    GPSChart(QWidget *parent = 0);
    ~GPSChart();
protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *event);
private:
    QMap<QString,QVector<QString> >map;
    QStringList PRNCode;
    QVector<QMap<QString,QRectF> >rects;//每颗卫星对应的标号和包含的矩形区域的矩形
    int radius;//星位图的半径
    int x,y;//鼠标当前的坐标

    QPixmap pixmap;//绘制卫星的信息

    QPointF center;
private:
    void getRects();//获得每颗卫星包含的举行区域
    bool isInRect(QString &label,const QPoint &point) const;//判断point是否在矩形区域内
public:
    void setMapAndRPNCode(const QMap<QString,QVector<QString> > &map,const QStringList &code);
};

#endif // GPSCHART_H
