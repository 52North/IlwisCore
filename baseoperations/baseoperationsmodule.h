#ifndef RASTERAPPLICATIONSMODULE_H
#define RASTERAPPLICATIONSMODULE_H

#include <QObject>
#include "module.h"
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"

namespace Ilwis {
namespace BaseOperations {
class BaseOperationsModule : public Module
{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit BaseOperationsModule(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString name() const;
    QString version() const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.baseoperations" FILE  "baseoperations.json")

};
}
}

#endif // RASTERAPPLICATIONSMODULE_H
