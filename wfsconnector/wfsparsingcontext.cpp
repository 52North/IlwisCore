#include <QString>
#include <QVariant>
#include <QMap>

#include "kernel.h"
#include "wfsparsingcontext.h"

using namespace Ilwis;
using namespace Wfs;

WfsParsingContext::WfsParsingContext()
{
}

QString WfsParsingContext::srsName() const
{
    return _srsName;
}

int WfsParsingContext::srsDimension() const
{
    return _srsDimension;
}

QString WfsParsingContext::geometryAtttributeName() const
{
    return _geometryAttributeName;
}

QMap<QString, QString> WfsParsingContext::namespaceMappings() const
{
    return _namespaceMappings;
}

QString WfsParsingContext::featureType() const
{
    return _featureType;
}

Resource WfsParsingContext::resource() const
{
    return _resource;
}

QString WfsParsingContext::currentItem() const
{
    return _currentItem;
}

void WfsParsingContext::setSrsName(QString srsName)
{
    _srsName = srsName;
}

void WfsParsingContext::setSrsDimension(int srsDimension)
{
    _srsDimension = srsDimension;
}

void WfsParsingContext::setGeometryAtttributeName(QString geometryAtttributeName)
{
    _geometryAttributeName = geometryAtttributeName;
}

void WfsParsingContext::addNamespaceMapping(QString prefix, QString uriNamespace)
{
    _namespaceMappings[prefix] = uriNamespace;
}

void WfsParsingContext::setFeatureType(QString featureType)
{
    _featureType = featureType;
}

void WfsParsingContext::setResource(Resource resource)
{
    _resource = resource;
}

void WfsParsingContext::setCurrentItem(QString item)
{
    _currentItem = item;
}



