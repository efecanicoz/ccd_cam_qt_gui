#include <common.h>
#include <QFile>

const QString file_name = "./config.csv";

void writeCfgFile(QList<config_str> &cfg_list)
{
    int i;
    QFile file(file_name);
    QString line;

    if(file.open(QIODevice::WriteOnly))
    {
        for(i = 0; i < cfg_list.count(); i++)
        {
            line = cfg_list[i].name + "," + QString::number(cfg_list[i].low) + "," + QString::number(cfg_list[i].high) + "\n";
            file.write(line.toUtf8());
        }
    }
    file.close();
}

void readCfgFile(QList<config_str> &cfg_list)
{
    config_str temp_cfg;
    QFile file(file_name);
    if(!file.open(QIODevice::ReadOnly))
    {
        //file not exists
        temp_cfg.name = "Config 1";
        temp_cfg.low = 100;
        temp_cfg.high = 700;
        cfg_list.append(temp_cfg);
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
            cfg_list.append(temp_cfg);
        }
    }
    file.close();
    return;
}
