#include "steerwheelwidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QDebug>

SteerWheelWidget::SteerWheelWidget(QWidget *parent):QWidget(parent)
  ,handleColor(Qt::red)
{
    _angle = 0;
    _currentAngle = 0;
    _radius = getRadius();
    _bReverse = false;
    _pivotRadius = 0;

    _updateTimer = new QTimer(this);
    _updateTimer->setInterval(Def::Number::UpdateInterval);
    connect(_updateTimer,SIGNAL(timeout()),this,SLOT(DoUpdateValue()));
}

void SteerWheelWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    _radius = getRadius();
    _pivotRadius = _radius / 9;

    painter.setRenderHints(QPainter::Antialiasing);
    drawBackground(&painter);
    drawPanel(&painter);
    drawHandle(&painter);
    drawPivot(&painter);
    drawMark(&painter);
    drawDirectionText(&painter);

}

void SteerWheelWidget::drawBackground(QPainter *painter)
{
    painter->save();
    QLinearGradient bgGradient(rect().topLeft(),rect().bottomLeft());
    bgGradient.setColorAt(0.0,Def::Color::Background_StartColor);
    bgGradient.setColorAt(1.0,Def::Color::Background_StopColor);
    painter->setBrush(bgGradient);
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect());
    painter->restore();
}

void SteerWheelWidget::drawDirectionText(QPainter *painter)
{
    int startAngle = 0;
    int dAngle = 90 ;


    painter->save();
    painter->setPen(QPen(Def::Color::Mark_Color));

    painter->translate(rect().center().x(),rect().center().y());

    QFont textFont;
    textFont.setBold(true);
    textFont.setPointSize(Def::Number::ExtraSpace * 2);
    painter->setFont(textFont);
    QString strText;

    for(int directionIndex = 0;directionIndex < Def::Number::DirectionCount;directionIndex++)
    {
        strText = tr("%1").arg(Def::Text::DirectionText[directionIndex]);
        painter->rotate(startAngle);
        painter->translate(QPoint(0,0));
        painter->drawText(QPoint(-(fontMetrics().width(strText)),_radius - Def::Number::ExtraSpace),strText);
        startAngle += dAngle;
    }
    painter->restore();


}

void SteerWheelWidget::drawPanel(QPainter *painter)
{
    painter->save();
    painter->setOpacity(1.0);
    painter->setPen(Def::Color::Circle_Color);
    QRadialGradient panelGradient(rect().center(),_radius,rect().center());
    panelGradient.setColorAt(0.0,Def::Color::Panel_StartColor);
    panelGradient.setColorAt(0.8,Def::Color::Panel_MiddleColor);
    panelGradient.setColorAt(1.0,Def::Color::Panel_StopColor);
    painter->setBrush(panelGradient);
    painter->drawEllipse(rect().center(),_radius,_radius);
    painter->restore();

}

void SteerWheelWidget::drawHandle(QPainter *painter)
{
    painter->save();
    painter->translate(rect().center().x(),rect().center().y());
    painter->drawText(-30,30,QString("偏正北方向%1度").arg(_angle));
    painter->rotate(_currentAngle % 360 - 90);

#if 0
//    painter->setPen(Def::Color::Handle_Color);
    painter->setPen(Qt::green);
    painter->drawLine(QPoint(0,0),QPoint(_radius - Def::Number::ExtraSpace * 2 - fontMetrics().height(),0));
#endif

    painter->setPen(Qt::NoPen);

    // draw north handle
    painter->save();
    QPointF topPoint(0,_pivotRadius );
    QPointF rightPoint(_radius - Def::Number::ExtraSpace * 2 - fontMetrics().height(),0);
    QPointF bottomPoint(0,- _pivotRadius);

    QLinearGradient northHandleGradient(topPoint,bottomPoint);
    northHandleGradient.setColorAt(0.0,handleColor);
    northHandleGradient.setColorAt(0.5,handleColor);
    northHandleGradient.setColorAt(1.0,handleColor);
    painter->setBrush(northHandleGradient);

    QVector<QPointF> pots;
    pots.push_back(topPoint);
    pots.push_back(rightPoint);
    pots.push_back(bottomPoint);
    painter->drawPolygon(QPolygonF(pots));
    painter->restore();

    painter->restore();

}

void SteerWheelWidget::drawPivot(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    QRadialGradient pivotGradient(rect().center(),_pivotRadius,rect().center());
    pivotGradient.setColorAt(0.0,Def::Color::Pivot_StartColor);
    pivotGradient.setColorAt(0.7,Def::Color::Pivot_StartColor);
    pivotGradient.setColorAt(1.0,Def::Color::Pivot_StopColor);
    painter->setBrush(pivotGradient);
    painter->drawEllipse(rect().center(),_pivotRadius,_pivotRadius);
    painter->restore();
}

void SteerWheelWidget::drawMark(QPainter *painter)
{
    painter->save();
    int dAngle = 360/Def::Number::MarkCount;
    int startAngle = 0;
    painter->translate(rect().center().x(),rect().center().y());
    painter->setPen(Def::Color::Mark_Color);
    for(int markIndex = 0;markIndex < Def::Number::MarkCount;markIndex++)
    {
        painter->rotate(startAngle);
        startAngle += dAngle;
        painter->drawLine(QPoint(_radius,0),QPoint(_radius - Def::Number::ExtraSpace * 0.75,0));
    }

    painter->restore();
}

int SteerWheelWidget::getRadius() const
{
    int shorterLen = width() > height() ? height() : width();
    int fontHeight = fontMetrics().height();
    int radius = (shorterLen - 2 * fontHeight - 4 * Def::Number::ExtraSpace)/2;
    return radius;
}

void SteerWheelWidget::initTimer()
{
    if(_updateTimer != NULL)
    {
        return ;
    }
    _updateTimer = new QTimer(this);
    _updateTimer->setInterval(Def::Number::UpdateInterval);
    connect(_updateTimer,SIGNAL(timeout()),this,SLOT(DoUpdateValue()));
}

void SteerWheelWidget::DoUpdateValue()
{
    if(_bReverse)
    {
        _currentAngle -= Def::Number::Delta;

        if(_currentAngle <= _angle)
        {
            _updateTimer->stop();
        }
    }
    else
    {
        _currentAngle += Def::Number::Delta;
        if(_currentAngle >= _angle)
        {
            _updateTimer->stop();
        }
    }

//    update();
    repaint();
}

void SteerWheelWidget::setAngle(int angle)
{
    if(angle > _angle)
    {
        _bReverse = false;
    }
    else
    {
        _bReverse = true;
    }
    _angle = angle;
    _currentAngle = angle;

    //如果需要连贯的动作，就把上面一句代码_currentAngle = angle;注释掉，把下面的代码取消注释
//    initTimer();
//    if(NULL != _updateTimer)
//         _updateTimer->start();
    repaint();
}

void SteerWheelWidget::setHandleColor(QColor color)
{
    handleColor = color;
}
