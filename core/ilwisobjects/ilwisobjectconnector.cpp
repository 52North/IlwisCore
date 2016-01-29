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

IlwisObjectConnector::IlwisObjectConnector(const Ilwis::Resource &resource, bool, const IOOptions &options) : _resource(resource), _binaryIsLoaded(false), _options(options)
{

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

void IlwisObjectConnector::setProperty(const QString &key, const QVariant &value)
{
    _resource.addProperty(key, value);
}

void IlwisObjectConnector::removeProperty(const QString &key)
{
    _resource.removeProperty(key);
}

bool IlwisObjectConnector::hasProperty(const QString &key)
{
    return _resource.hasProperty(key);
}

const Resource& IlwisObjectConnector::source() const{
    return _resource;
}

bool IlwisObjectConnector::dataIsLoaded() const
{
    return _binaryIsLoaded;
}

IOOptions IlwisObjectConnector::ioOptions() const
{
    return _options;
}

IOOptions &IlwisObjectConnector::ioOptionsRef()
{
    return _options;
}

