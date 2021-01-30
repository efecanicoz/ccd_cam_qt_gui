#ifndef CFG_H
#define CFG_H

#include <QDialog>
#include <QList>
#include "common.h"
#include "cfgnamewindow.h"

namespace Ui {
class Dialog;
}

class cfgWindow : public QDialog
{
    Q_OBJECT
signals:
    void list_updated();

public:
    explicit cfgWindow(QWidget *parent = nullptr, QList<config_str> *cl = nullptr);
    ~cfgWindow();
    QList<config_str> *config_list;;

protected:
    void showEvent( QShowEvent* event );

public slots:
    void addNewCfg();
    void go_to_main();
    void write_file();
    void deleteCurrentCfg();

private:
    QWidget *main_window;
    Ui::Dialog *ui;
    void save();
    cfgNameWindow *nameWindow;

};

#endif // CFG_H
