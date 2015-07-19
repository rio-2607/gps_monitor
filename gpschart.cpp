#include "gpschart.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QPalette>
#include <QPointF>
#include <QtMath>
#include <QDebug>
#include <QFont>
#include <QMap>
#include <QColor>
#include <QString>
#include <QVector>
#include <QMapIterator>
#define PI 3.1415926
#define CIRCLE_RADIUS 10//每颗卫星的半径

GPSChart::GPSChart(QWidget *parent)
    : QWidget(parent),
      pixmap(QPixmap(60,35))
{
    //设置灰色背景
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window,Qt::gray);
    palette.setBrush(QPalette::Window,QBrush(QPixmap(":/earth1.png")));
    this->setPalette(palette);
//    resize(300,300);
    setFixedSize(350,300);

    int width = size().width();
    int height = size().height();
    center = QPointF(width / 2,height / 2);

    radius = qMin(width-40,height-40) / 2;

    setMouseTracking(true);
//    setAutoFillBackground(true);
}

GPSChart::~GPSChart()
{

}

void GPSChart::paintEvent(QPaintEvent *)
{
//    getRects();//每次重绘之后都要调用getRects()函数，重新获取卫星所在的矩形区域
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);//设置反锯齿效果
    painter.setPen(QPen(QColor(124,223,141)));
//    painter.setPen(QPen(Qt::black));

    int width = size().width();
    int height = size().height();
    center = QPointF(width / 2,height / 2);

    radius = qMin(width-40,height-40) / 2;

    QPointF newCenter(0,0);
    painter.save();

    painter.translate(center);//把绘图的坐标轴远点转移到圆心
    //绘制同心圆
    painter.drawEllipse(newCenter,radius,radius);
    painter.drawEllipse(newCenter,radius * qSin(30 * PI / 180),radius * qSin(30 * PI / 180));
    painter.drawEllipse(newCenter,radius * qSin(45 * PI / 180),radius * qSin(45 * PI / 180));

    painter.save();

    //绘制横纵斜坐标轴
//    QPen pen(QColor(102,102,102),2);
    QPen pen(Qt::yellow);
    QFont serialFont("Times",14,QFont::Bold);
    painter.setFont(serialFont);
    painter.setPen(pen);
    //横轴
    painter.drawLine(-radius,0,radius,0);
    painter.drawText(radius+2,0,QString("90"));
    painter.drawText(-radius - 35,0,QString("270"));
    //纵轴
    painter.drawLine(0,-radius,0,radius);
    painter.drawText(-8,-radius,QString("0"));;
    painter.drawText(-20,radius + 18,QString("180"));

    painter.rotate(45);
    painter.drawLine(-radius,0,radius,0);
    painter.drawText(radius+2,0,QString("135"));
    painter.drawText(-radius - 35,0,QString("315"));

    painter.drawLine(0,-radius,0,radius);
    painter.drawText(-8,-radius,QString("45"));
    painter.drawText(-20,radius + 18,QString("225"));

    painter.restore();

    painter.save();

    painter.setPen(QPen(Qt::black));

    QMap<QString,QVector<QString> >::const_iterator iter = map.constBegin();
    while(iter != map.constEnd())
    {
        int elevate,zaimuth,newRadius,centerX,centerY;
        if(!iter.value().at(2).isEmpty())
        {
            elevate = iter.value().at(0).toInt();
            zaimuth = iter.value().at(1).toInt();
            newRadius = radius * qSin(elevate * PI / 180);
            centerX = newRadius * qSin(zaimuth * PI / 180);
            centerY = -1 * newRadius * qCos(zaimuth * PI / 180);
            painter.setBrush(QColor(85,170,255));
            //绘制出这个卫星
            QPointF c(centerX,centerY);
            painter.drawEllipse(c,CIRCLE_RADIUS,CIRCLE_RADIUS);
            painter.drawText(centerX-5,centerY+5,iter.key());//绘制卫星标号
        }
        ++iter;
    }
    painter.restore();

    painter.restore();

    painter.drawPixmap(x,y,pixmap);

}

void GPSChart::setMapAndRPNCode(const QMap<QString,QVector<QString> > &map,const QStringList &code)
{
    this->map = map;
    this->PRNCode = code;
    getRects();

    update();
}

void GPSChart::mouseMoveEvent(QMouseEvent *event)
{
    x = event->x();
    y = event->y();

    pixmap.fill(QColor(255,255,255,100));

    QString label;
    if(isInRect(label,event->pos()))
    {
        QPainter painter(&pixmap);
        painter.drawText(0,20,
                         QString("  仰角:%1").arg(map.value(label).at(0)));
        painter.drawText(0,30,
                         QString("方位角:%1").arg(map.value(label).at(1)));
    }
    else
        pixmap.fill(QColor(255,255,255,0));

    update();
}

//获得每颗卫星所包含的矩形区域
void GPSChart::getRects()
{
    rects.clear();//每次调用这个函数前都要把rects清空
    QMap<QString,QRectF> tempMap;
    QRectF rect;
    QMap<QString,QVector<QString> >::const_iterator iter = map.constBegin();
    int width = size().width();
    int height = size().height();
    QPointF center(width / 2,height / 2);

    while(iter != map.constEnd())
    {
        if(!iter.value().at(2).isEmpty())//如果该卫星的信噪比不为空，则说明该颗卫星有信号
        {
            tempMap.clear();//注意，这句代码必须添加，tempMap每次使用前必须清零
            int elevate = iter.value().at(0).toInt();
            int zaimuth = iter.value().at(1).toInt();
            int newRadius = radius * qSin(elevate * PI / 180);
            //圆心坐标
            int centerX = center.x() + newRadius * qSin(zaimuth * PI / 180);
            int centerY = center.y() - newRadius * qCos(zaimuth * PI / 180);
            rect = QRectF(centerX - CIRCLE_RADIUS,centerY - CIRCLE_RADIUS,
                          2 * CIRCLE_RADIUS,2 * CIRCLE_RADIUS);
            tempMap.insert(iter.key(),rect);
            rects.push_back(tempMap);

        }
        ++iter;
    }
}


//判断鼠标当前位置是否位于卫星的矩形区域内
//如果位于卫星的范围内，返回true，并且把该颗卫星的标号保存在label中
bool GPSChart::isInRect(QString &label,const QPoint &point) const
{
    QVector<QMap<QString,QRectF> >::const_iterator iter = rects.constBegin();
    QMap<QString,QRectF> map1;
    QMap<QString,QRectF>::const_iterator iter1;
    QRectF r;
    while(iter != rects.constEnd())
    {
        map1 = *iter;
        iter1 = map1.constBegin();
        r = iter1.value();
        if(r.contains(point))
        {
            label = iter1.key();
            return true;
        }
        ++iter;
    }
    return false;
}
