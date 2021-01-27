#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QUdpSocket>
#include <QOpenGLWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QGLCanvas : public QOpenGLWidget
{
public:
    QGLCanvas(QWidget* parent = NULL);
    void setImage(const QImage& image);
protected:
    void paintEvent(QPaintEvent*);
private:
    QImage img;
};

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    void run(void);
    QByteArray fb;
    QImage *raw_image;
private:
    QUdpSocket *socket = nullptr;

signals:
    void img_updated();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void fb_callback();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    WorkerThread *workerThread;
    QGLCanvas widget;
};

#endif // MAINWINDOW_H
