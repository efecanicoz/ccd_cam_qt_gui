#ifndef CFGNAMEWINDOW_H
#define CFGNAMEWINDOW_H

#include <QDialog>

namespace Ui {
class cfgNameWindow;
}

class cfgNameWindow : public QDialog
{
    Q_OBJECT

public:
    explicit cfgNameWindow(QWidget *parent = nullptr, QString *name=nullptr);
    ~cfgNameWindow();
    QString *input_name;

public slots:
    void save_quit();
    void cancel_quit();

private:
    Ui::cfgNameWindow *ui;
};

#endif // CFGNAMEWINDOW_H
