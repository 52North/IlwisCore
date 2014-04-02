#include <QString>
#include "kernel.h"
#include "abstractfactory.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"

using namespace Ilwis;

IlwisObjectFactory::IlwisObjectFactory(const QString &ty, const QString &sub, const QString &desc) : AbstractFactory(ty,sub,desc)
{
}

IlwisObject *IlwisObjectFactory::createObject(IlwisObjectConnector* connector, const PrepareOptions &options) const {
    IlwisObject *object = connector->create();
    if ( object) {
        bool ok = connector->loadMetaData(object, options);
        if (ok) {
            object->setValid(true);
            object->setConnector(connector);
            return object;
        }
    }else {
        kernel()->issues()->log(TR("Could not create object"));
    }
    return 0;

}
