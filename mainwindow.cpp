#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtEndian>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDateTime>
#include <QFile>
#include <QSlider>

void WorkerThread::run()
{
    QHostAddress sender;
    u_int16_t port;
    bool locked = false;
    u_int8_t frag_no;
    QByteArray framebuffer;
    QByteArray datagram;
    int i;


    fb.append(QString("P6 2700 1 255 "));
    fb.resize(8114); //2700 * 3 + 14(header)
    //for(i = 0; i < 8100; i++)
    //    fb[i + 14] = i%255;//framebuffer[2*i+1];
    //ret = raw_image->loadFromData(fb);

    socket = new QUdpSocket();
    socket->bind(QHostAddress::AnyIPv4, 34567);
    if (socket->state() != socket->BoundState)
        qDebug() << "Not bound";
    else
        qDebug() << "Bound";

    framebuffer.resize(16200);

    while(1)
    {
        socket->waitForReadyRead(-1);
        while (socket->hasPendingDatagrams())
        {
            datagram.resize(socket->pendingDatagramSize());
            socket->readDatagram(datagram.data(),datagram.size(),&sender,&port);

            frag_no = datagram[0];

            if(locked == false && frag_no == 0)
            {
                locked = true;
            }
            if(locked == true)
            {
                if(frag_no != 16)
                    framebuffer.replace((frag_no*1000), 1000, (datagram.data()+1),1000);
                else
                {
                    qDebug() << QDateTime::currentDateTime().toString("mm:ss,zzz");
                    framebuffer.replace((frag_no*1000), 100, (datagram.data()+1),200);
                    for(i = 0; i < (8100 - 1); i++)
                    {
                        quint16 tmp_u16;
                        tmp_u16 = framebuffer[i]<<8 | framebuffer[i+1];
                        tmp_u16 = tmp_u16 << 2;
                        fb[i+14] = (quint8)tmp_u16; //framebuffer[2*i+1];
                    }
                    raw_image->loadFromData(fb);
                    emit img_updated(); //let openglcanvas draw it
                    qDebug() << QDateTime::currentDateTime().toString("mm:ss,zzz");
                    qDebug() << "frame taken";

                }
            }
        }
    }
}

void MainWindow::fb_callback()
{
    qDebug() << "fb cb";
    widget.setImage(*workerThread->raw_image);
    this->widget.update();
}

void MainWindow::initGPIO()
{
    QFile exportFile("/sys/class/gpio/export");
    exportFile.open(QIODevice::WriteOnly);
    exportFile.write("4");
    exportFile.close();

    QFile directionFile("/sys/class/gpio/gpio4/direction");
    directionFile.open(QIODevice::WriteOnly);
    directionFile.write("out");
    directionFile.close();

    QFile stateFile("/sys/class/gpio/gpio4/value");
    stateFile.open(QIODevice::WriteOnly);
    stateFile.write("1");
    stateFile.close();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    int i;

    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);


    workerThread = new WorkerThread();
    workerThread->raw_image = new QImage(2700, 1, QImage::Format_RGB888);
    workerThread->start();

    settings_window = new cfgWindow(this, &config_list);

    widget.setGeometry(0,0,900,50);


    //fill the ddl
    readCfgFile(config_list);
    for(i = 0; i < config_list.count(); i++)
        ui->ddl_configuration->addItem(config_list[i].name);

    //widget.show();
    connect(ui->pb_on_off, SIGNAL(clicked()), this, SLOT(onOffEvent()));
    connect(ui->pb_run, SIGNAL(clicked()), this, SLOT(show_widget()));
    connect(ui->pb_stop, SIGNAL(clicked()), this, SLOT(hide_widget()));
    connect(ui->pb_settings, SIGNAL(clicked()), this, SLOT(show_settings()));
    connect(workerThread, SIGNAL(img_updated()), this, SLOT(fb_callback()));
    connect(settings_window, SIGNAL(list_updated()), this, SLOT(update_ddl_list()));
    connect(ui->slider_speed, SIGNAL(sliderReleased()), this, SLOT(update_slider_text()));
}

void MainWindow::update_ddl_list()
{
    int i;

    //first remove everything
        ui->ddl_configuration->clear();

    for(i = 0; i < config_list.count(); i++)
        ui->ddl_configuration->addItem(config_list[i].name);

}

void MainWindow::show_settings()
{
    settings_window->show();
    hide();
}

void MainWindow::show_widget()
{

    QFile stateFile("/sys/class/gpio/gpio4/value");
    stateFile.open(QIODevice::WriteOnly);
    stateFile.write("0");
    stateFile.close();

    this->widget.show();
}

void MainWindow::hide_widget()
{

    QFile stateFile("/sys/class/gpio/gpio4/value");
    stateFile.open(QIODevice::WriteOnly);
    stateFile.write("1");
    stateFile.close();

    this->widget.hide();
}

void MainWindow::onOffEvent()
{
    QApplication::quit();
}

void MainWindow::update_slider_text()
{
    ui->lbl_speed->setText(QString("Speed(%")+ QString::number(ui->slider_speed->value()) + QString(")"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

QGLCanvas::QGLCanvas(QWidget* parent) : QOpenGLWidget(parent)
{
}

void QGLCanvas::setImage(const QImage& image)
{
    img = image;
}

void QGLCanvas::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    //Set the painter to use a smooth scaling algorithm.
    p.setRenderHint(QPainter::SmoothPixmapTransform, 1);

    p.drawImage(this->rect(), img);
}

