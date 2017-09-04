#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "inifile.h"
#include "ilwis3connector.h"
#include "representation.h"
#include "ilwis3representationconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

Ilwis3RepresentationConnector::~Ilwis3RepresentationConnector()
{

}

Ilwis3RepresentationConnector::Ilwis3RepresentationConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) :
    Ilwis3Connector(resource, load, options)
{

}

bool Ilwis3RepresentationConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    Ilwis3Connector::loadMetaData(data, options);

    QString type = _odf->value("Representation","Type");
    Representation *rpr = static_cast<Representation *>(data);
    if ( type == "RepresentationGradual"){
        return parseRprGradual(rpr);
    }else if ( type == "RepresentationClass") {
        return parseRprClass();
    }
    return false;
}

bool Ilwis3RepresentationConnector::storeMetaData(IlwisObject *obj, const IOOptions &options)
{
    return false;
}

IlwisObject *Ilwis3RepresentationConnector::create() const
{
    return new Representation();
}

ConnectorInterface *Ilwis3RepresentationConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
    return new Ilwis3RepresentationConnector(resource, load, options);
}

QString Ilwis3RepresentationConnector::format() const
{
    return "representation";
}

bool Ilwis3RepresentationConnector::parseRprClass()
{
    return false;
}

bool Ilwis3RepresentationConnector::parseRprGradual(Representation* rpr)
{
    bool ok;
    int boundaryWidth = _odf->value("Representation","BoundaryWidth").toInt(&ok);
    if ( ok){

    }
    return false;
}

