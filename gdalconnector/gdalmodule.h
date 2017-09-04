#ifndef GDALMODULE_H
#define GDALMODULE_H

#include <QObject>
#include <QLibrary>
#include "module.h"
#include "kernel.h"


namespace Ilwis {
namespace Gdal {

class GdalModule : public Module
{
public:
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit GdalModule(QObject *parent = 0);
    ~GdalModule();

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    void getOperations(QVector<ICommandInfo *>& commands) const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.gdalconnector" FILE  "gdalconnector.json")

private:


signals:
};
}
}

#endif // GDALMODULE_H
