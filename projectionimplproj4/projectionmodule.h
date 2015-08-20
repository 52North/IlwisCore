#ifndef PROJ_MODULE
#define PROJ_MODULE

#include <QObject>
#include <QLibrary>
#include "module.h"
#include "kernel.h"
//#include "factory.h"
//#include "abstractfactory.h"

namespace Ilwis {

class ProjectionModule : public Module
{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit ProjectionModule(QObject *parent = 0);

    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.projectionimplproj4" FILE  "projectionImplProj4.json")

signals:

public slots:

};
}

#endif // PROJ_MODULE
