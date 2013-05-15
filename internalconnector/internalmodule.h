#ifndef ILWIS3MODULE_H
#define ILWIS3MODULE_H

#include <QObject>
#include "module.h"
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"

namespace Ilwis {
namespace Internal {

class InternalModule : public Module
{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit InternalModule(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString name() const;
    QString version() const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.internalconnector" FILE  "internalconnector.json")

};
}
}

#endif // ILWIS3MODULE_H
