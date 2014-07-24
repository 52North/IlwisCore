#include <QUrl>
#include "kernel.h"
#include "errorobject.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"

using namespace Ilwis;

IlwisObjectConnector::IlwisObjectConnector(const Ilwis::Resource &resource, bool, const IOOptions &) : _resource(resource), _binaryIsLoaded(false)
{
    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ConnectorFactory",resource);

    if ( factory && resource.url().isValid() &&
         resource.container().isValid() &&
         mastercatalog()->usesContainers(resource.url())){
         _incontainerconnector.reset(dynamic_cast<CatalogConnector *>(factory->createContainerConnector(Resource(resource.container().url(), itCATALOG))));
    }
}

IlwisObjectConnector::~IlwisObjectConnector()
{

}

IlwisTypes IlwisObjectConnector::type() const
{
    return _resource.ilwisType();
}

Resource &IlwisObjectConnector::source()
{
    return _resource;
}

const Resource& IlwisObjectConnector::source() const{
    return _resource;
}

bool IlwisObjectConnector::dataIsLoaded() const
{
    return _binaryIsLoaded;
}

UPCatalogConnector &IlwisObjectConnector::containerConnector(IlwisObject::ConnectorMode mode)
{
    if ( hasType(mode,IlwisObject::cmINPUT) && _incontainerconnector)
        return _incontainerconnector;
    else if ( hasType(mode ,IlwisObject::cmOUTPUT) && _outcontainerconnector)
        return _outcontainerconnector;
    if (!_incontainerconnector)
        throw ErrorObject(TR("Using uninitialized container"));

    return _incontainerconnector;
}

const UPCatalogConnector &IlwisObjectConnector::containerConnector(IlwisObject::ConnectorMode mode) const
{
    if ( hasType(mode,IlwisObject::cmINPUT) && _incontainerconnector)
        return _incontainerconnector;
    else if ( hasType(mode,IlwisObject::cmOUTPUT) && _outcontainerconnector)
        return _outcontainerconnector;
    if (!_incontainerconnector)
        throw ErrorObject(TR("Using uninitialized container"));

    return _incontainerconnector;
}
