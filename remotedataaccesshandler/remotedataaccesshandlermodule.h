#ifndef RemoteDataAccessHandlerMODULE_H
#define RemoteDataAccessHandlerMODULE_H

#include <QObject>
#include "module.h"
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"

namespace Ilwis {
namespace RemoteDataAccess {

class RemoteDataAccessHandlerModule : public Module
{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit RemoteDataAccessHandlerModule(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    virtual void finalizePreparation();

    Q_PLUGIN_METADATA(IID "n52.ilwis.remotedataaccesshandler" FILE  "remotedataaccesshandler.json")

    void prepare();
signals:

public slots:

};
}
}

#endif
