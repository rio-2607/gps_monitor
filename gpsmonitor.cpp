#include "gpsmonitor.h"
#include "ui_gpsmonitor.h"
#include "iconhelper.h"
#include "myhelper.h"
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QPalette>
#include <QDesktopWidget>
//#include <QColorGroup>

GPSMonitor::GPSMonitor(QWidget *parent) :
     QDialog(parent),
    ui(new Ui::GPSMonitor),
    gpsChart(new GPSChart),
    startX(0)
{
    ui->setupUi(this);

    myHelper::FormInCenter(this);
    this->InitStyle();

    //设置主窗口灰色背景
    QPalette palette1 = this->palette();
    palette1.setColor(QPalette::Window,Qt::gray);
    this->setPalette(palette1);

    thread = new DetectedSerialPortThread(QSerialPortInfo::availablePorts());
    thread->start();//开启检测串口设备线程

    //控件初始化
    QLabel *portNameLabel = new QLabel(tr("端口"));
    portNameCombo = new QComboBox;

   QLabel *baudRateLabel = new QLabel(tr("波特率"));
   baudRateCombo = new QComboBox;

   QLabel *parityLabel = new QLabel(tr("校验位"));
   parityCombo = new QComboBox;

   QLabel *dataBitsLabel = new QLabel(tr("数据位"));
   dataBitsCombo = new QComboBox;

   QLabel *stopBitsLabel = new QLabel(tr("停止位"));
   stopBitsCombo = new QComboBox;

   satelliteNumLabel = new QLabel(tr("卫星:  颗"));
   statusLabel = new QLabel(tr("状态:    "));
   modeLabel = new QLabel("模式:  ");
   styleLabel = new QLabel(tr("类型:    "));
   highLabel = new QLabel(tr("海拔:   米"));
   latitudeLabel = new QLabel(tr("纬度:"));
   longitudeLabel = new QLabel(tr("经度: "));

   QVBoxLayout *labelLayout = new QVBoxLayout;
   labelLayout->addWidget(satelliteNumLabel);
   labelLayout->addWidget(statusLabel);
   labelLayout->addWidget(modeLabel);
   labelLayout->addWidget(styleLabel);
   labelLayout->addWidget(highLabel);
   labelLayout->addWidget(latitudeLabel);
   labelLayout->addWidget(longitudeLabel);

   QGridLayout *gridLayout = new QGridLayout;
   gridLayout->addWidget(portNameLabel,0,0);
   gridLayout->addWidget(portNameCombo,0,1);
   gridLayout->addWidget(baudRateLabel,1,0);
   gridLayout->addWidget(baudRateCombo,1,1);
   gridLayout->addWidget(parityLabel,2,0);
   gridLayout->addWidget(parityCombo,2,1);
   gridLayout->addWidget(dataBitsLabel,3,0);
   gridLayout->addWidget(dataBitsCombo,3,1);
   gridLayout->addWidget(stopBitsLabel,4,0);
   gridLayout->addWidget(stopBitsCombo,4,1);

   initComboBox();

   dataBrowser = new QTextBrowser;
   //设置dataBrowser背景色和前景色
   QPalette palette = dataBrowser->palette();
   palette.setColor(QPalette::Base,Qt::black);
   palette.setColor(QPalette::Text,Qt::gray);
   dataBrowser->setPalette(palette);

   QHBoxLayout *leftTopLayout = new QHBoxLayout;
   leftTopLayout->addLayout(gridLayout);
   leftTopLayout->addSpacing(50);
   leftTopLayout->addLayout(labelLayout);
   leftTopLayout->addWidget(dataBrowser);

   speedWatch = new mySpeedWatch;
   steerWheelWidget = new SteerWheelWidget;
   steerWheelWidget->setHandleColor(Qt::gray);//设置起始状态为灰色

   //添加两个自定义的仪表盘
   QVBoxLayout *customWidget = new QVBoxLayout;
   customWidget->addWidget(speedWatch);
   customWidget->addWidget(gpsChart);

   //初始化PRN控件
   for(int i = 0; i < NUMBER_OF_SATELLITE;++i)
       indicator[i] = new myIndicator;

   QGridLayout *PRNLayout = new QGridLayout;
   for(int i = 0; i < COLUMN_NUM;++i)
      PRNLayout->addWidget(indicator[i],0,i);
   for(int i = COLUMN_NUM;i < NUMBER_OF_SATELLITE;++i)
      PRNLayout->addWidget(indicator[i],1,i - COLUMN_NUM);


   openSerialPortButton = new QPushButton(tr("打开串口"));
   openFileButton = new QPushButton(tr("打开文件"));

   QHBoxLayout *buttonLayout = new QHBoxLayout;
   buttonLayout->addWidget(openSerialPortButton);
   buttonLayout->addWidget(openFileButton);

   QVBoxLayout *layout = new QVBoxLayout;
   layout->addLayout(leftTopLayout);
   layout->addLayout(buttonLayout);
   layout->addLayout(PRNLayout);

   QHBoxLayout *mainLayout = new QHBoxLayout;
   mainLayout->addLayout(layout);
   mainLayout->addLayout(customWidget);

   //生成 误差曲线图
   highErrorPlot = new QCustomPlot;
   longitudeErrorPlot = new QCustomPlot;
   latitudeErrorPlot = new QCustomPlot;
   totalDiffPlot = new QCustomPlot;//总的误差曲线
   dotPlot = new QCustomPlot;

   //初始化设置
   initPlot();

   //误差曲线图包含在表格布局里面
   QGridLayout *plotLayout = new QGridLayout;
   plotLayout->addWidget(highErrorPlot,0,0);
   plotLayout->addWidget(longitudeErrorPlot,0,1);
   plotLayout->addWidget(totalDiffPlot,1,0);
   plotLayout->addWidget(latitudeErrorPlot,1,1);

   highEdit = new QLineEdit();
   highEdit->setToolTip(tr("请输入精确高度"));
   highEdit->setPlaceholderText(tr("请输入精确高度"));
   longitudeEdit = new QLineEdit();
   longitudeEdit->setToolTip(tr("请输入精确经度"));
   longitudeEdit->setPlaceholderText(tr("请输入精确经度"));
   latitudeEdit = new QLineEdit();
   latitudeEdit->setPlaceholderText(tr("请输入精确纬度"));
   latitudeEdit->setToolTip(tr("请输入精确纬度"));


   QHBoxLayout *editLayout = new QHBoxLayout;
   editLayout->addWidget(highEdit);
   editLayout->addWidget(longitudeEdit);
   editLayout->addWidget(latitudeEdit);

   QVBoxLayout *rightLayout = new QVBoxLayout;

   QHBoxLayout *rightTopLayout = new QHBoxLayout;
   rightTopLayout->addWidget(steerWheelWidget);
   rightTopLayout->addWidget(dotPlot);

   rightLayout->addLayout(rightTopLayout);
   rightLayout->addLayout(editLayout);
   rightLayout->addLayout(plotLayout);

   //把右边的布局加入主布局中
   mainLayout->addLayout(rightLayout);

   ui->centralWidget->setLayout(mainLayout);

   state = Closed;

   connect(thread,SIGNAL(serialPortChanged(bool,QString)),
            this,SLOT(onSerialPortChanged(bool,QString)));
   connect(&receiveDataThread,SIGNAL(error(QString)),
            this,SLOT(onSerialPortError(QString)));
   connect(&receiveDataThread,SIGNAL(request(QString)),
            this,SLOT(onSerialPortRequeat(QString)));
   connect(&receiveDataThread,SIGNAL(success()),
            this,SLOT(onSerialportSuccess()));

   connect(openSerialPortButton,SIGNAL(clicked()),
            this,SLOT(onOpenSerialPort()));

   connect(&resolveNMEA,SIGNAL(dateUpdate(QStringList,QStringList,int,QStringList,
                                           QMap<QString,QVector<QString> >,float,float,
                                          int,float,QString,int)),
            this,SLOT(onDataUpdate(QStringList,QStringList,int,
                                   QStringList,QMap<QString,QVector<QString> >,float,float,
                                   int,float,QString,int)));
   connect(openFileButton,SIGNAL(clicked()),this,SLOT(onOpenFile()));

   //移动到桌面左上角
   move(0,0);
   //获取桌面大小
   QDesktopWidget *desktop = QApplication::desktop();
   QRect desktopRect = desktop->availableGeometry();
   //设置界面大小等于桌面大小
   resize(desktopRect.width(),desktopRect.height());
}

GPSMonitor::~GPSMonitor()
{
    delete ui;
}

void GPSMonitor::InitStyle()
{
    //设置窗体标题栏隐藏
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    location = this->geometry();
    max = false;
    mousePressed = false;

    //安装事件监听器,让标题栏识别鼠标双击
    ui->lab_Title->installEventFilter(this);

    IconHelper::Instance()->SetIcon(ui->btnMenu_Close, QChar(0xf00d), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Min, QChar(0xf068), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu, QChar(0xf0c9), 10);
    IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf015), 12);
}

bool GPSMonitor::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->on_btnMenu_Max_clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void GPSMonitor::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void GPSMonitor::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void GPSMonitor::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void GPSMonitor::on_btnMenu_Close_clicked()
{
//    receiveDataThread.stop();
    qApp->exit();
}

void GPSMonitor::on_btnMenu_Max_clicked()
{
    if (max) {
        this->setGeometry(location);
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
        ui->btnMenu_Max->setToolTip("最大化");
    } else {
        location = this->geometry();
        this->setGeometry(qApp->desktop()->availableGeometry());
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf079), 10);
        ui->btnMenu_Max->setToolTip("还原");
    }
    max = !max;
}

void GPSMonitor::on_btnMenu_Min_clicked()
{
    this->showMinimized();
}


void GPSMonitor::onSerialPortChanged(bool state, QString name)
{
    QString s;
    if(state)
        s = QString("检测到新插入串口,新插入串口为:");
    else
        s = QString("检测到串口被拔出,拔出串口为:");


    myHelper::ShowMessageBoxInfo(s + name);

    resetPortNameCombo();
}


void GPSMonitor::onSerialPortError(QString/* error*/)
{
//    qDebug() << "There is error occured:" + error << endl;
    myHelper::ShowMessageBoxError(tr("打开串口失败"));
}

void GPSMonitor::onSerialPortRequeat(QString data)
{
    //把每次接收到的文件都保存在以接收时间命名的dat文件中
    dataBrowser->append(data);
    resolveNMEA.resolve(data);
//    QStringList list = QDateTime::currentDateTime().toString().split(" ");
//    QString fileName = list.at(2) + list.at(3) + QString(".dat");
//    fileName.replace(":","");
//    QFile outputFile(fileName);
//    if(!outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
//    {
//        myHelper::ShowMessageBoxError(tr("新建文件失败"));
//        outputFile.close();
//        return;
//    }

    QTextStream out(&outputFile);
    out << data;
//    outputFile.close();

}


void GPSMonitor::initComboBox()
{
    //设置串口
   foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
       portNameCombo->addItem(info.portName());
   //设置波特率
   baudRateCombo->addItem(QString("1200"));
   baudRateCombo->addItem(QString("2400"));
   baudRateCombo->addItem(QString("4800"));
   baudRateCombo->addItem(QString("9600"));
   baudRateCombo->addItem(QString("19200"));
   baudRateCombo->addItem(QString("38400"));
   baudRateCombo->addItem(QString("57600"));
   baudRateCombo->addItem(QString("115200"));
   baudRateCombo->setCurrentIndex(3);//设置当前值为9600
   //设置校验位
   parityCombo->addItem(QString("无校验"));
   parityCombo->addItem(QString("奇校验"));
   parityCombo->addItem(QString("偶校验"));
   //设置数据位
   dataBitsCombo->addItem(QString("5"));
   dataBitsCombo->addItem(QString("6"));
   dataBitsCombo->addItem(QString("7"));
   dataBitsCombo->addItem(QString("8"));
   dataBitsCombo->setCurrentIndex(3);;//设置当前值为8
   //设置停止位
   stopBitsCombo->addItem(QString("1"));
   stopBitsCombo->addItem(QString("1.5"));
   stopBitsCombo->addItem(QString("2"));

}


void GPSMonitor::onOpenSerialPort()
{
    if(Closed == state)//如果当前串口处于关闭状态
    {
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        int baudRates[] = {1200,2400,4800,9600,
                           19200,38400,57600,115200};
        QSerialPort::DataBits dataBits[] = {QSerialPort::Data5,QSerialPort::Data6,
                                       QSerialPort::Data7,QSerialPort::Data8};
        QSerialPort::Parity parity[] = {QSerialPort::NoParity,QSerialPort::OddParity,
                                        QSerialPort::EvenParity};
        QSerialPort::StopBits stopBits[] = {QSerialPort::OneStop,QSerialPort::OneAndHalfStop,
                                            QSerialPort::TwoStop};
        receiveDataThread.startReceiveData(ports.at(portNameCombo->currentIndex()).portName(),
                                           baudRates[baudRateCombo->currentIndex()],
                                           parity[parityCombo->currentIndex()],
                                           stopBits[stopBitsCombo->currentIndex()],
                                           dataBits[dataBitsCombo->currentIndex()]);

        //每次在打开串口成功后把 误差向量清空
        highDiff.clear();
        longitudeDiff.clear();
        latitudeDiff.clear();
        totalDiff.clear();
        dotX.clear();
        dotY.clear();

        //禁能输入框
        highEdit->setEnabled(false);
        longitudeEdit->setEnabled(false);
        latitudeEdit->setEnabled(false);

        //打开文件，保存发送过来的数据
        QStringList list = QDateTime::currentDateTime().toString().split(" ");
        QString fileName = list.at(2) + list.at(3) + QString(".dat");
        fileName.replace(":","");
        outputFile.setFileName(fileName);
        if(!outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            myHelper::ShowMessageBoxError(tr("新建文件失败"));
            outputFile.close();
        }
    }
    else
    {
        receiveDataThread.setQuitFlag(true);
        state = Closed;
        openSerialPortButton->setText(tr("打开串口"));
        enabledCombo(true);
        //把表盘置零
//        speedWatch->setValue(0);
//        steerWheelWidget->setAngle(0);
//        //RPN控件清零
//        for(int i = 0;i < NUMBER_OF_SATELLITE;++i)
//        {
//            indicator[i]->setValue(0);
//            indicator[i]->setLabelValue("");
//        }
//        //把各个标签控件置零
//        satelliteNumLabel->setText(tr("卫星： 颗"));
//        statusLabel->setText(tr("状态:"));
//        modeLabel->setText(tr("模式:"));;
//        styleLabel->setText(tr("类型:"));
//        highLabel->setText(tr("海拔: 米"));
//        latitudeLabel->setText(tr("纬度:"));
//        longitudeLabel->setText(tr("经度:"));

        //使能输入框
        highEdit->setEnabled(true);
        longitudeEdit->setEnabled(true);
        latitudeEdit->setEnabled(true);

        //关闭文件
        outputFile.close();
    }


}


void GPSMonitor::onSerialportSuccess()
{
    openSerialPortButton->setText(tr("关闭串口"));
    state = Opened;//设置串口状态为 打开
    enabledCombo(false);
}


void GPSMonitor::resetPortNameCombo()
{
    portNameCombo->clear();
    //设置串口
   foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
       portNameCombo->addItem(info.portName());
}


void GPSMonitor::enabledCombo(bool enabled)
{
    portNameCombo->setEnabled(enabled);
    baudRateCombo->setEnabled(enabled);
    parityCombo->setEnabled(enabled);
    dataBitsCombo->setEnabled(enabled);
    stopBitsCombo->setEnabled(enabled);
}

void GPSMonitor::onDataUpdate(QStringList latitude,QStringList longitude,
                              int numOfSatellite,QStringList PRNCode,
                              QMap<QString,QVector<QString> > info,
                              float groundSpeed,float zaimuth,int status,
                              float high,QString mode,int style)
{

    //横坐标压栈并加一
    x.push_back(startX++);

    //获取用户输入的精准坐标
    QString accurateHightStr = highEdit->text();
    QString accurateLatitudeStr = latitudeEdit->text();
    QString accurateLongitudeStr = longitudeEdit->text();

    //数字经纬度
    double accurateHigh = accurateHightStr.toDouble();
    double accuratelatitude = str2double(accurateLatitudeStr);
    double accurateLongitude = str2double(accurateLongitudeStr);

    //获取客户端发送过来的经纬度信息
    double measureHigh = (double) high;
    double measureLatitude = latitude.at(0).toDouble();
    double measureLongitude = longitude.at(0).toDouble();

    double highError = (measureHigh - accurateHigh) * 100 / (accurateHigh == 0 ? measureHigh : accurateHigh);
    double latitudeError = (measureLatitude - accuratelatitude) * 100 /
            (accuratelatitude == 0 ? measureLatitude : accuratelatitude);
    double longitudeError = (measureLongitude - accurateLongitude) * 100 /
            (accurateLongitude == 0 ? measureLongitude : accurateLongitude);
    double totalError = sqrt(highError * highError +
                             latitudeError * latitudeError + longitudeError * longitudeError);

    //计算打点图中的横纵坐标的误差
    double errorY = measureLatitude - accuratelatitude;
    double errorX = measureLongitude - accurateLongitude;
    //把小数表示的数据全部转换为以(度)°为单位
    double dotErrorX = double2du(errorX);

    double dotErrorY = double2du(errorY);
    //把dotErrorX保留两位小数
    double dotErrorX1 = floor((dotErrorX - floor(dotErrorX)) * 100) / 100 + floor(dotErrorX);

    //把计算得到的数据保存到数据结构中
    highDiff.push_back(highError);
    longitudeDiff.push_back(longitudeError);
    latitudeDiff.push_back(latitudeError);
    totalDiff.push_back(totalError);
    dotX.push_back(dotErrorX1);
    dotY.push_back(dotErrorY);

    //画图
    highErrorPlot->graph()->setData(x,highDiff);
    highErrorPlot->replot();
    highErrorPlot->graph()->rescaleAxes();//注意，这句代码一定要添加，不然的话会看不到数据

    longitudeErrorPlot->graph()->setData(x,longitudeDiff);
    longitudeErrorPlot->graph()->rescaleAxes();
    longitudeErrorPlot->replot();

    latitudeErrorPlot->graph()->setData(x,latitudeDiff);
    latitudeErrorPlot->graph()->rescaleAxes();
    latitudeErrorPlot->replot();

    totalDiffPlot->graph()->setData(x,totalDiff);
    totalDiffPlot->graph()->rescaleAxes();
    totalDiffPlot->replot();

    dotPlot->graph()->setData(dotX,dotY);
    dotPlot->graph()->rescaleAxes();
    dotPlot->replot();

    //设置仪表盘数据,如果速度小于0.2m/s,即0.39节，则方向盘指向正北，且以灰色表示
    speedWatch->setValue(groundSpeed);
    if(groundSpeed < 0.39)
    {
        steerWheelWidget->setHandleColor(Qt::gray);
        steerWheelWidget->setAngle(0);
    }
    else
    {
        steerWheelWidget->setHandleColor(Qt::red);
        steerWheelWidget->setAngle(zaimuth);
    }
    //显示PRN信噪比数值
    QMap<QString,QVector<QString> >::const_iterator iter = info.constBegin();
    int index = 0;
    while(iter != info.constEnd())
    {
        indicator[index]->setLabelValue(iter.key());
        indicator[index]->setValue(iter.value().at(2).toInt());
        ++index;
        ++iter;
    }

    satelliteNumLabel->setText(QString("卫星:%1%2颗").arg("  ").arg(numOfSatellite));
    highLabel->setText(QString("海拔:  %1米").arg(high));
    if("M" == mode)
        modeLabel->setText(QString("模式:  自动"));
    else
        modeLabel->setText(QString("模式:  手动"));
    if(2 == style)
        styleLabel->setText(QString("类型：二维定位"));
    else if(3 == style)
        styleLabel->setText(QString("类型：三维定位"));
    else
        styleLabel->setText(QString("类型： 未定位"));

    if(1 == status)
        statusLabel->setText(QString("状态:定为有效"));
    else
        statusLabel->setText(QString("状态:定为有效"));

    QString latitudeStr = getItude(latitude);
    latitudeLabel->setText(QString("纬度:") + latitudeStr);
    QString longitudestr = getItude(longitude);
    longitudeLabel->setText(QString("经度:") + longitudestr);

    gpsChart->setMapAndRPNCode(info,PRNCode);//显示星位图

    repaint();//注意，这里一定是repaint函数，要强制重绘

    //每次接受完成后使能输入框
    highEdit->setEnabled(true);
    longitudeEdit->setEnabled(true);
    latitudeEdit->setEnabled(true);
}

QString GPSMonitor::getItude(const QStringList &strData)
{
//    QString result;
    float data = strData.at(0).toFloat();

    float du = floor(data / 100);
    float data1 = data - du * 100;
    float minutes = floor(data1);
    float second = floor((data1 - minutes) * 60);

    QString dataStr = QString("%1度%2分%3秒").arg(du).arg(minutes).arg(second);

    QString direction;
    if("N" == strData.at(1))
        direction = "北纬";
    else if("S" == strData.at(1))
        direction = "南纬";
    else if("E" == strData.at(1))
        direction = "东经";
    else
        direction = "西经";
    return direction + dataStr;
}


void GPSMonitor::onOpenFile()
{
    if(Opened == state)
    {
        myHelper::ShowMessageBoxError(tr("通信中，无法打开文件"));
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(this,tr("选择文件"),
                                                    ".",tr("数据文件 (*.dat)"));
    if(fileName.isEmpty())
        return;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        myHelper::ShowMessageBoxError(tr("打开文件错误"));
        return;
    }

    QString data = file.readAll();
    //把打开的数据显示在浏览器中
    dataBrowser->append(data);
    resolveNMEA.resolve(data);
    file.close();

    //每次在打开文件成功后把 误差向量清空
    highDiff.clear();
    longitudeDiff.clear();
    latitudeDiff.clear();
    totalDiff.clear();

    dotX.clear();
    dotY.clear();

    //禁能输入框
    highEdit->setEnabled(false);
    longitudeEdit->setEnabled(false);
    latitudeEdit->setEnabled(false);
}


//初始化曲线图
void GPSMonitor::initPlot()
{
    highErrorPlot->xAxis2->setVisible(true);
    highErrorPlot->xAxis2->setTickLabels(false);
    highErrorPlot->yAxis2->setVisible(true);
    highErrorPlot->yAxis2->setTickLabels(false);
    highErrorPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    //设置X轴Y轴范围
    highErrorPlot->yAxis->setRange(-1,1);
    highErrorPlot->xAxis->setRange(0,200);
    highErrorPlot->yAxis->setLabel("误差:%");
    connect(highErrorPlot->xAxis,SIGNAL(rangeChanged(QCPRange)),
            highErrorPlot->xAxis2,SLOT(setRange(QCPRange)));
    connect(highErrorPlot->yAxis,SIGNAL(rangeChanged(QCPRange)),
            highErrorPlot->yAxis2,SLOT(setRange(QCPRange)));
    //添加标题
    highErrorPlot->plotLayout()->insertRow(0);
    highErrorPlot->plotLayout()->addElement(0,0,new QCPPlotTitle(highErrorPlot,"高度误差曲线图"));

    longitudeErrorPlot->xAxis2->setVisible(true);
    longitudeErrorPlot->xAxis2->setTickLabels(false);
    longitudeErrorPlot->yAxis2->setVisible(true);
    longitudeErrorPlot->yAxis2->setTickLabels(false);
    longitudeErrorPlot->setInteractions(QCP::iRangeDrag |
                                          QCP::iRangeZoom | QCP::iSelectPlottables);
    //设置X轴Y轴范围
    longitudeErrorPlot->yAxis->setRange(-1,1);
    longitudeErrorPlot->xAxis->setRange(0,200);
    longitudeErrorPlot->yAxis->setLabel("误差:%");
    connect(longitudeErrorPlot->xAxis,SIGNAL(rangeChanged(QCPRange)),
            longitudeErrorPlot->xAxis2,SLOT(setRange(QCPRange)));
    connect(longitudeErrorPlot->yAxis,SIGNAL(rangeChanged(QCPRange)),
            longitudeErrorPlot->yAxis2,SLOT(setRange(QCPRange)));
    //添加标题
    longitudeErrorPlot->plotLayout()->insertRow(0);
    longitudeErrorPlot->plotLayout()->addElement(0,0,new QCPPlotTitle(longitudeErrorPlot,"经度误差曲线图"));

    latitudeErrorPlot->xAxis2->setVisible(true);
    latitudeErrorPlot->xAxis2->setTickLabels(false);
    latitudeErrorPlot->yAxis2->setVisible(true);
    latitudeErrorPlot->yAxis2->setTickLabels(false);
    latitudeErrorPlot->setInteractions(QCP::iRangeDrag |
                                          QCP::iRangeZoom | QCP::iSelectPlottables);
    //设置X轴Y轴范围
    latitudeErrorPlot->yAxis->setRange(-1,1);
    latitudeErrorPlot->xAxis->setRange(0,200);
    latitudeErrorPlot->yAxis->setLabel("误差:%");
    connect(latitudeErrorPlot->xAxis,SIGNAL(rangeChanged(QCPRange)),
            latitudeErrorPlot->xAxis2,SLOT(setRange(QCPRange)));
    connect(latitudeErrorPlot->yAxis,SIGNAL(rangeChanged(QCPRange)),
            latitudeErrorPlot->yAxis2,SLOT(setRange(QCPRange)));
    //添加标题
    latitudeErrorPlot->plotLayout()->insertRow(0);
    latitudeErrorPlot->plotLayout()->addElement(0,0,new QCPPlotTitle(latitudeErrorPlot,"纬度误差曲线图"));


    totalDiffPlot->xAxis2->setVisible(true);
    totalDiffPlot->xAxis2->setTickLabels(false);
    totalDiffPlot->yAxis2->setVisible(true);
    totalDiffPlot->yAxis2->setTickLabels(false);
    totalDiffPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    //设置X轴Y轴范围
    totalDiffPlot->yAxis->setRange(-1,1);
    totalDiffPlot->xAxis->setRange(0,200);
    totalDiffPlot->yAxis->setLabel("误差:%");
//    totalDiffPlot->addGraph();
    connect(totalDiffPlot->xAxis,SIGNAL(rangeChanged(QCPRange)),
            totalDiffPlot->xAxis2,SLOT(setRange(QCPRange)));
    connect(totalDiffPlot->yAxis,SIGNAL(rangeChanged(QCPRange)),
            totalDiffPlot->yAxis2,SLOT(setRange(QCPRange)));
    //添加标题
    totalDiffPlot->plotLayout()->insertRow(0);
    totalDiffPlot->plotLayout()->addElement(0,0,new QCPPlotTitle(totalDiffPlot,"总的误差曲线图"));

    dotPlot->xAxis2->setVisible(true);
    dotPlot->xAxis2->setTickLabels(false);
    dotPlot->yAxis2->setVisible(true);
    dotPlot->yAxis2->setTickLabels(false);
    dotPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    //设置X轴Y轴范围
    dotPlot->yAxis->setRange(-100,100);
    dotPlot->xAxis->setRange(-100,100);
    connect(dotPlot->xAxis,SIGNAL(rangeChanged(QCPRange)),
            dotPlot->xAxis2,SLOT(setRange(QCPRange)));
    connect(dotPlot->yAxis,SIGNAL(rangeChanged(QCPRange)),
            dotPlot->yAxis2,SLOT(setRange(QCPRange)));
    //添加标题
    dotPlot->plotLayout()->insertRow(0);
    dotPlot->plotLayout()->addElement(0,0,new QCPPlotTitle(dotPlot,"打点图"));

    //往各个plot里面添加图形
    highErrorPlot->addGraph();
    highErrorPlot->graph()->setPen(QPen(QColor(255,100,0)));

    longitudeErrorPlot->addGraph();
    longitudeErrorPlot->graph(0)->setPen(QPen(QColor(255,100,0)));

    latitudeErrorPlot->addGraph();
    latitudeErrorPlot->graph(0)->setPen(QPen(QColor(255,100,0)));

    totalDiffPlot->addGraph();
    latitudeErrorPlot->graph(0)->setPen(QPen(QColor(255,100,0)));

    dotPlot->addGraph();
    dotPlot->graph()->setLineStyle(QCPGraph::lsNone);
    dotPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,4));
}


//把字符表示的数据转化为十进制的数字表示的经纬度
//公式是 X°Y′Z″ ------> X * 100 + Y + Z / 60
double GPSMonitor::str2double(const QString &str)
{
    double result = 0;
    if(!str.isEmpty())
    {
        QStringList strList = str.split(" ");
        result = strList.at(0).toDouble() * 100 +
                strList.at(2).toDouble() / 60 + strList.at(1).toDouble();
    }
    return result;
}


//把小数表示的经纬度数据转换为以度(°)为单位的数据
//1° = 60′ = 3600″
double GPSMonitor::double2du(const double data)
{
    double du = floor(data / 100);
    double data1 = data - du * 100;
    double minutes = floor(data1);
    double second = floor((data1 - minutes) * 60);

    double result = du + minutes / 60 + second / 3600;
    return result;
}
