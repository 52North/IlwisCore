#include "kernel.h"
#include "ilwisdata.h"
#include "gdalproxy.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "gdalfeaturetableconnector.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "coverage.h"
#include "attributedefinition.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "feature.h"
#include "basetable.h"
#include "flattable.h"
#include "attributetable.h"
#include "gdaltableloader.h"

using namespace Ilwis;
using namespace Gdal;

GdalFeatureTableConnector::GdalFeatureTableConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : GdalConnector(resource, load, options){}

ConnectorInterface *GdalFeatureTableConnector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new GdalFeatureTableConnector(resource, load, options);

}

IlwisObject* GdalFeatureTableConnector::create() const{
    bool astable = ioOptions().contains("asflattable");
    if ( astable == false && hasType(_resource.extendedType(), itFEATURE))
        return new AttributeTable(_resource);
    return new FlatTable(_resource);
}


bool GdalFeatureTableConnector::loadMetaData(IlwisObject* data, const IOOptions& options){
    if(!GdalConnector::loadMetaData(data, options))
        return false;

    OGRLayerH hLayer = getLayerHandle();
    if ( hLayer){
        GdalTableLoader loader;
        loader.loadMetaData((Table *)data, hLayer);
    }
    return true;
}

bool GdalFeatureTableConnector::storeMetaData(Ilwis::IlwisObject *obj){
    return true;
}

bool GdalFeatureTableConnector::loadData(IlwisObject * data, const IOOptions &){
    if(!GdalConnector::loadMetaData(data, IOOptions())) // ??
        return false;

    Table *attTable = static_cast<Table *>(data);

    OGRLayerH hLayer = getLayerHandle();
    bool ok = true;
    if ( hLayer) {
        std::vector<QVariant> record(attTable->columnCount());

        GdalTableLoader loader;
        loader.setColumnCallbacks(attTable, hLayer);
        OGRFeatureH hFeature = 0;
        gdal()->resetReading(hLayer);
        //each FEATURE
        try {
            quint32 index = 0;
            while( (hFeature = gdal()->getNextFeature(hLayer)) != NULL){
                loader.loadRecord(attTable, hFeature, record);
                attTable->record(attTable->recordCount(), record);
                gdal()->destroyFeature( hFeature );
                attTable->record(index++, record);
            }
            _binaryIsLoaded = true;
            return true;
        } catch (FeatureCreationError& ) {
            gdal()->destroyFeature( hFeature );
            ok = false;
        }
    }
    gdal()->closeFile(_fileUrl.toLocalFile(), data->id());
    return ok;
}



bool GdalFeatureTableConnector::storeBinaryData(IlwisObject* obj){
    return true;
}
