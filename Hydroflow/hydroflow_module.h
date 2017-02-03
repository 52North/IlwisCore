#ifndef HYDROFLOW_MODULE_H
#define HYDROFLOW_MODULE_H

#include <QObject>
#include <QLibrary>
#include "module.h"
#include "kernel.h"

namespace Ilwis {
namespace Hydroflow {

class Hydroflow_module  : public Module{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit Hydroflow_module(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString name() const;
    QString version() const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.hydroflow" FILE  "hydroflow.json")


signals:

};
}
}

#endif // HYDROFLOW_MODULE_H
