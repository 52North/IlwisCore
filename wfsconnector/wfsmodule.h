#ifndef WFSMODULE_H
#define WFSMODULE_H

#include <QObject>
#include <QLibrary>
#include "kernel.h"


namespace Ilwis {
namespace Wfs {

class WfsModule : public Module
{
public:
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit WfsModule(QObject *parent = 0);
    ~WfsModule();

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    void getOperations(QVector<ICommandInfo *>& ) const {}
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.wfsconnector" FILE  "wfsconnector.json")

private:


signals:
};
}
}

#endif // WFSMODULE_H
