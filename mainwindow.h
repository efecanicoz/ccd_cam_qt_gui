#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QUdpSocket>
#include <QOpenGLWidget>
#include "common.h"
#include "cfgWindow.h"

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
    void onOffEvent();
    void show_widget();
    void hide_widget();
    void show_settings();
    void update_ddl_list();
    void update_slider_text(int value);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    WorkerThread *workerThread;
    QGLCanvas widget;
    QList<config_str> config_list;
    cfgWindow *settings_window;
    void initGPIO();

};

#endif // MAINWINDOW_H
