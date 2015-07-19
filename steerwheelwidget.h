#ifndef GPSWIDGET_H
#define GPSWIDGET_H
#include <QWidget>
#include <QColor>
namespace Def
{
    namespace Color
    {
        const QColor Background_StartColor = QColor(122,122,122);
        const QColor Background_StopColor = QColor(65,65,65);

        const QColor Panel_StartColor = QColor(221,221,221);
        const QColor Panel_MiddleColor = QColor(221,221,221);
        const QColor Panel_StopColor = QColor(189,189,189);

        const QColor Pivot_StartColor = QColor(165,165,165);
        const QColor Pivot_StopColor = QColor(67,67,67);

        const QColor Text_Color = QColor(Qt::black);
        const QColor Circle_Color = QColor(Qt::black);
        const QColor Handle_Color = Qt::black;
        const QColor Mark_Color = Qt::black;

        const QColor SouthDirection_StartColor = QColor(236,11,11);
        const QColor SouthDirection_StopColor = QColor(234,48,48);

        const QColor NorthDirection_StartColor = QColor(154,154,154);
        const QColor NorthDirection_StopColor = QColor(132,132,132);
    }

    namespace Number
    {
        const int ExtraSpace = 10;
        const int UpdateInterval = 1;
        const int DirectionCount = 4;
        const int Delta = 1;
        const int MarkCount = 8;
    }

    namespace Text
    {
        const char DirectionText[] = {'S','W','E','N'};
    }
}

class QPainter;
class QPaintEvent;

class SteerWheelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SteerWheelWidget(QWidget* parent = 0);
    void setHandleColor(QColor color);

public Q_SLOTS:
    void setAngle(int angle);

protected:
    void paintEvent(QPaintEvent *);

    QSize sizeHint() const
    {
        return QSize(200,200);
    }

    QSize minimumSizeHint() const
    {
        return QSize(120,120);
    }


private:
    void drawBackground(QPainter* painter);
    void drawDirectionText(QPainter* painter);
    void drawPanel(QPainter* painter);
    void drawHandle(QPainter* painter);
    void drawPivot(QPainter* painter);
    void drawMark(QPainter* painter);

    int getRadius() const;
    void initTimer();

private Q_SLOTS:
    void DoUpdateValue();

private:

    QTimer* _updateTimer;
    bool _bReverse;
    int _angle;
    int _currentAngle;
    int _radius;
    qreal _pivotRadius;
    QColor handleColor;//句柄颜色


};

#endif // GPSWIDGET_H
