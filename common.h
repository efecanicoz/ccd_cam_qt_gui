#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QList>

typedef struct {
    QString name;
    quint16 low;
    qint16 high;
}config_str;

extern void writeCfgFile(QList<config_str> &cfg_list);
extern void readCfgFile(QList<config_str> &cfg_list);

#endif // COMMON_H
