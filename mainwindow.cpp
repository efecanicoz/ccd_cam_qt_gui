#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

void WorkerThread::run()
{
    QHostAddress sender;
    u_int16_t port;
    bool locked = false;
    u_int8_t frag_no;
    QByteArray framebuffer;
    QByteArray datagram;

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
                    framebuffer.replace((frag_no*1000), 100, (datagram.data()+1),200);
            }
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    workerThread = new WorkerThread();
    workerThread->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}

