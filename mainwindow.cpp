#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtEndian>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDateTime>
#include <QFile>

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    workerThread = new WorkerThread();
    workerThread->raw_image = new QImage(2700, 1, QImage::Format_RGB888);
    workerThread->start();

    widget.setGeometry(0,0,900,50);

    connect(workerThread, SIGNAL(img_updated()), this, SLOT(fb_callback()));

    readCfgFile();
    //widget.show();
    connect(ui->pb_on_off, SIGNAL(clicked()), this, SLOT(onOffEvent()));
    connect(ui->pb_run, SIGNAL(clicked()), this, SLOT(show_widget()));
    connect(ui->pb_stop, SIGNAL(clicked()), this, SLOT(hide_widget()));
    connect(ui->pb_new, SIGNAL(clicked()), this, SLOT(addNewCfg()));

}

void MainWindow::addNewCfg()
{
    config_str temp_cfg;
    temp_cfg.name = "Unnamed config";
    temp_cfg.low = 100;
    temp_cfg.high = 700;
    config_list.append(temp_cfg);
    this->ui->ddl_configuration->addItem(temp_cfg.name);
    this->ui->ddl_configuration->setCurrentIndex(this->ui->ddl_configuration->count() -1);

    writeCfgFile();
}

void MainWindow::writeCfgFile()
{
    int i;
    QFile file("./config.csv");
    QString line;

    if(file.open(QIODevice::WriteOnly))
    {
        for(i = 0; i < config_list.count(); i++)
        {
            line = config_list[i].name + "," + QString::number(config_list[i].low) + "," + QString::number(config_list[i].high) + "\n";
            file.write(line.toUtf8());
        }
    }
    file.close();
}

void MainWindow::readCfgFile()
{
    config_str temp_cfg;
    QFile file("./config.csv");
    if(!file.open(QIODevice::ReadOnly))
    {
        //file not exists
        temp_cfg.name = "Config 1";
        temp_cfg.low = 100;
        temp_cfg.high = 700;
        this->config_list.append(temp_cfg);
        this->ui->ddl_configuration->addItem(temp_cfg.name);
    }
    else
    {
        while(!file.atEnd())
        {
            QString line = file.readLine();
            QStringList fields = line.split(",");
            temp_cfg.name = fields[0];
            temp_cfg.low = fields[1].toUInt();
            temp_cfg.high = fields[2].toUInt();
            this->ui->ddl_configuration->addItem(fields[0]);
            this->config_list.append(temp_cfg);
        }
    }
    file.close();
    return;
}

void MainWindow::show_widget()
{
    this->widget.show();
}

void MainWindow::hide_widget()
{
    this->widget.hide();
}

void MainWindow::onOffEvent()
{
    QApplication::quit();
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

