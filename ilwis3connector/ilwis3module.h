#ifndef ILWIS3MODULE_H
#define ILWIS3MODULE_H

#include <QObject>
#include "module.h"
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"

namespace Ilwis {
namespace Ilwis3{

class Ilwis3Module : public Module
{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit Ilwis3Module(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString name() const;
    QString version() const;
    void getOperations(QVector<ICommandInfo *>& commands) const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.ilwis3connector" FILE  "ilwis3connector.json")

};
}
}

#endif // ILWIS3MODULE_H
