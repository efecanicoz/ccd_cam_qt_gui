#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QList>
#include <stdint.h>
#include <stddef.h>

typedef uint16_t u_int16_t;
typedef uint8_t u_int8_t;
typedef size_t size_type;

typedef struct {
    QString name;
    quint16 low;
    qint16 high;
}config_str;

extern void writeCfgFile(QList<config_str> &cfg_list);
extern void readCfgFile(QList<config_str> &cfg_list);

#endif // COMMON_H
