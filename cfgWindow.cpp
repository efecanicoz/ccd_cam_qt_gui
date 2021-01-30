#include "cfgWindow.h"
#include "ui_cfgWindow.h"

cfgWindow::cfgWindow(QWidget *parent, QList<config_str> *cl) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    this->main_window = parent;
    this->config_list = cl;
    ui->setupUi(this);

    connect(ui->pb_new, SIGNAL(clicked()), this, SLOT(addNewCfg()));
    connect(ui->pb_delete, SIGNAL(clicked()), this, SLOT(deleteCurrentCfg()));
    connect(ui->pb_exit, SIGNAL(clicked()), this, SLOT(go_to_main()));
    connect(ui->pb_save, SIGNAL(clicked()), this ,SLOT(write_file()));

}

void cfgWindow::write_file()
{
    writeCfgFile(*config_list);
}

void cfgWindow::go_to_main()
{
    emit list_updated();
    this->main_window->show();
    this->hide();
}

void cfgWindow::showEvent( QShowEvent* event ) {
    QWidget::showEvent( event );
    int i;

    this->ui->ddl_configuration->clear();

    for(i = 0; i < config_list->count(); i++)
        this->ui->ddl_configuration->addItem(config_list->at(i).name);

    return;
}

cfgWindow::~cfgWindow()
{
    delete ui;
}

void cfgWindow::deleteCurrentCfg()
{
    int index;
    index = this->ui->ddl_configuration->currentIndex();

    this->ui->ddl_configuration->removeItem(index);
    this->config_list->removeAt(index);
}

void cfgWindow::addNewCfg()
{
    int ret_status;
    config_str temp_cfg;
    QString name;
    this->nameWindow = new cfgNameWindow(this, &name);
    ret_status = this->nameWindow->exec();

    if(0 == ret_status) //user cancelled
        return;

    temp_cfg.name = name;
    temp_cfg.low = 100;
    temp_cfg.high = 700;
    config_list->append(temp_cfg);
    this->ui->ddl_configuration->addItem(temp_cfg.name);
    this->ui->ddl_configuration->setCurrentIndex(this->ui->ddl_configuration->count() -1);

    writeCfgFile(*config_list);
}
