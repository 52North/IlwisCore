
#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "domainitem.h"
#include "ilwisdata.h"
#include "domain.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "numericrange.h"
#include "range.h"
#include "itemrange.h"
#include "identifierrange.h"
#include "gdalproxy.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "numericdomain.h"
#include "domainconnector.h"


using namespace Ilwis;
using namespace Gdal;

ConnectorInterface *DomainConnector::create(const Resource& resource, bool load, const IOOptions &options) {
    return new DomainConnector(resource, load, options);

}

DomainConnector::DomainConnector(const Resource& resource, bool load, const IOOptions &options) : GdalConnector(resource,load, options)
{
}

bool DomainConnector::loadMetaData(IlwisObject *data, const IOOptions &options){
    bool ret = true;
    if (!GdalConnector::loadMetaData(data, options))
        ret = false;

    if (type() == itUNKNOWN) {
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("Domain type",data->name()));
        ret = false;
    }

    if (ret == true){
        if ( type() == itNUMERICDOMAIN) {
            ret = handleValueDomains(data);
        } else if ( type() == itITEMDOMAIN) {
            ret = handleThematicDomains(data);
        }
    }
    QFileInfo fileinf (sourceRef().toLocalFile());
    gdal()->closeFile(fileinf.absoluteFilePath(), data->id());
    return ret;

}

bool DomainConnector::handleThematicDomains(IlwisObject* ) {
    //TODO: handle thematic raster domains
    return false;
}

bool DomainConnector::handleValueDomains(IlwisObject* data) {
    //NumericDomain *vdata = static_cast<NumericDomain*>(data);
    //TODO: adjust the range
    return true;
}

IlwisObject *DomainConnector::create() const
{
    //TODO: other domain types time, identifier
    //TODO: what about other system domain types that are value derived.
    if ( type() == itNUMERICDOMAIN)
        return new NumericDomain(_resource);
    //else if ( type() == itTHEMATICDOMAIN) {
    //    return new ItemDomain<ThematicItem>(_resource);
    //}
    return 0;
}
