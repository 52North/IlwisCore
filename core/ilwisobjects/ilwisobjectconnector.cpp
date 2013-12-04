#include <QUrl>
#include "kernel.h"
#include "errorobject.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectconnector.h"

using namespace Ilwis;

IlwisObjectConnector::IlwisObjectConnector(const Ilwis::Resource &resource, bool) : _resource(resource)
{
    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ConnectorFactory",resource);

    if ( factory && resource.url().isValid()){
         //incontainerconnector.reset(dynamic_cast<ContainerConnector *>(factory->createSuitable(Resource(resource.url(), itCONTAINER))));
         _incontainerconnector.reset(dynamic_cast<ContainerConnector *>(factory->createSuitable(Resource(resource.container().url(), itCONTAINER))));
    }
    else {
        kernel()->issues()->log(TR("Cann't find suitable factory for %1 ").arg(resource.name()));
    }
}

IlwisTypes IlwisObjectConnector::type() const
{
    return _resource.ilwisType();
}

Resource &IlwisObjectConnector::source()
{
    return _resource;
}

std::unique_ptr<ContainerConnector> &IlwisObjectConnector::containerConnector(IlwisObject::ConnectorMode mode)
{
    if ( hasType(mode,IlwisObject::cmINPUT) && _incontainerconnector)
        return _incontainerconnector;
    else if ( hasType(mode ,IlwisObject::cmOUTPUT) && _outcontainerconnector)
        return _outcontainerconnector;
    if (!_incontainerconnector)
        throw ErrorObject(TR("Using uninitialized container"));

    return _incontainerconnector;
}

const std::unique_ptr<ContainerConnector> &IlwisObjectConnector::containerConnector(IlwisObject::ConnectorMode mode) const
{
    if ( hasType(mode,IlwisObject::cmINPUT) && _incontainerconnector)
        return _incontainerconnector;
    else if ( hasType(mode,IlwisObject::cmOUTPUT) && _outcontainerconnector)
        return _outcontainerconnector;
    if (!_incontainerconnector)
        throw ErrorObject(TR("Using uninitialized container"));

    return _incontainerconnector;
}
