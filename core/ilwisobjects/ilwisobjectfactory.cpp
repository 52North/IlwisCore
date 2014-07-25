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

IlwisObject *IlwisObjectFactory::createObject(IlwisObjectConnector* connector, const IOOptions &options) const {
    IlwisObject *object = connector->create();
    if ( object) {
        object->setValid(true);
        object->setConnector(connector, IlwisObject::cmINPUT, options);
        return object;
    }else {
        kernel()->issues()->log(TR("Could not create object"));
    }
    return 0;

}
