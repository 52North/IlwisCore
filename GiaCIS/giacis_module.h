#ifndef GIACIS_MODULE_H
#define GIACIS_MODULE_H

#include <QObject>
#include <QLibrary>
#include "module.h"
#include "kernel.h"

namespace Ilwis {
namespace GiaCIS {

class GiaCIS_module  : public Module{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit GiaCIS_module(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString name() const;
    QString version() const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.giacis" FILE  "giacis.json")


signals:

};
}
}

#endif // GIACIS_MODULE_H
