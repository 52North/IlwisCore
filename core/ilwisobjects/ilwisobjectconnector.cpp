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

Resource &IlwisObjectConnector::sourceRef()
{
    return _resource;
}

const Resource &IlwisObjectConnector::source() const
{
    return _resource;
}

void IlwisObjectConnector::addProperty(const QString &key, const QVariant &value)
{
    if ( key == "connectormode"){
        _mode = (IlwisObject::ConnectorMode)value.toInt();

    }else
        _resource.addProperty(key, value);
}

void IlwisObjectConnector::removeProperty(const QString &key)
{
    _resource.removeProperty(key);
}

bool IlwisObjectConnector::hasProperty(const QString &key) const
{
    return _resource.hasProperty(key);
}

QVariant IlwisObjectConnector::getProperty(const QString &name) const
{
    if ( hasProperty(name))
        return _resource[name];
    return QVariant();
}

const Resource& IlwisObjectConnector::sourceRef() const{
    return _resource;
}

bool IlwisObjectConnector::dataIsLoaded() const
{
    return _binaryIsLoaded;
}

void IlwisObjectConnector::unload()
{

}

IOOptions IlwisObjectConnector::ioOptions() const
{
    return _options;
}

IOOptions &IlwisObjectConnector::ioOptionsRef()
{
    return _options;
}

