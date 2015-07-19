#include "gpsmonitor.h"
#include <QApplication>
#include "myhelper.h"

#include <QList>
#include <QString>

#include <QStringList>
#include "myspeedwatch.h"
#include "steerwheelwidget.h"
#include "myindicator.h"

#include "gpschart.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    myHelper::SetUTF8Code();
    myHelper::SetStyle("black");//黑色风格
//    myHelper::SetStyle("blue");//蓝色风格
//    myHelper::SetStyle("gray");//灰色风格
//    myHelper::SetStyle("navy");//天蓝色风格
    myHelper::SetChinese();

    GPSMonitor w;
//    GPSChart w;

    w.show();

    return a.exec();

}
