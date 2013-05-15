#ifndef ILWIS3MODULE_H
#define ILWIS3MODULE_H

#include <QObject>
#include "module.h"
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"

namespace Ilwis {

class ProjectionModule : public Module
{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit ProjectionModule(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString name() const;
    QString version() const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.projectionimplproj4" FILE  "projectionImplProj4.json")

signals:

public slots:

};
}

#endif // ILWIS3MODULE_H
