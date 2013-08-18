#ifndef _H
#define _H

#include <QObject>
#include <QLibrary>
#include "module.h"
#include "kernel.h"

namespace Ilwis {

class RasterOperationsModule  : public Module{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit RasterOperationsModule(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString name() const;
    QString version() const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.rasteroperations" FILE  "rasteroperations.json")


signals:

};
}

#endif // _H
