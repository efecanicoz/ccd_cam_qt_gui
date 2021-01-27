#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtEndian>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDateTime>

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

    connect(workerThread, SIGNAL(img_updated()), this, SLOT(fb_callback()));

    widget.setGeometry(0,0,900,50);
    widget.setImage(*workerThread->raw_image);
    widget.show();
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

