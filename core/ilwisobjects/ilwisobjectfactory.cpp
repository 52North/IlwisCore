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

std::vector<Resource> IlwisObjectFactory::loadResource(const QUrl &url, IlwisTypes types) const
{
    return std::vector<Resource>();
}

IlwisObject *IlwisObjectFactory::createObject(IlwisObjectConnector* connector, const IOOptions &options) const {
    IlwisObject *object = connector->create();
    if ( object) {
        object->setValid(true);
        if (!object->setConnector(connector, IlwisObject::cmINPUT, options))
            return 0;

        return object;
    }else {
        kernel()->issues()->log(TR("Could not create object"));
    }
    return 0;

}
