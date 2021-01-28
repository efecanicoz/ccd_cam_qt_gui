#include "cfgnamewindow.h"
#include "ui_cfgnamewindow.h"

cfgNameWindow::cfgNameWindow(QWidget *parent, QString *name) :
    QDialog(parent),
    ui(new Ui::cfgNameWindow)
{
    ui->setupUi(this);
    input_name = name;

    connect(ui->pb_exit, SIGNAL(clicked()), this, SLOT(cancel_quit()));
    connect(ui->pb_save, SIGNAL(clicked()), this, SLOT(save_quit()));
}

void cfgNameWindow::save_quit()
{
    *input_name = ui->tb_name->text();
    this->accept();
}

void cfgNameWindow::cancel_quit()
{
    this->reject();
}

cfgNameWindow::~cfgNameWindow()
{
    delete ui;
}
