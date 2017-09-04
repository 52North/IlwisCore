#include "coverage.h"
#include "version.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "coverageserializerv1.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"
#include "featureserializerv1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *FeatureSerializerV1::create(QDataStream& stream)
{
    return new FeatureSerializerV1(stream);
}

FeatureSerializerV1::FeatureSerializerV1(QDataStream &stream) : CoverageSerializerV1(stream)
{
}

bool FeatureSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    if (!CoverageSerializerV1::store(obj, options))
        return false;

    IFeatureCoverage fcoverage;
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");

    fcoverage.prepare(obj->id());
    //TODO per level, at the moment only flat featurecoverages
    qint32 fcount = fcoverage->featureCount(itPOINT);
    _stream << fcount;
    fcount = fcoverage->featureCount(itLINE);
    _stream << fcount;
    fcount = fcoverage->featureCount(itPOLYGON);
   _stream << fcount;


    int defCount = fcoverage->attributeDefinitions().definitionCount();
    _stream << defCount;

    for(int col = 0; col < defCount; ++col){
        const ColumnDefinition& coldef = fcoverage->attributeDefinitionsRef().columndefinitionRef(col);
        _stream << coldef.name();
        _stream << coldef.datadef().domain()->valueType();

        std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::interfaceVersion, itDOMAIN,_stream));
        if ( !domainStreamer)
            return false;
        storeSystemPath(coldef.datadef().domain()->resource());
        domainStreamer->store(coldef.datadef().domain().ptr(), options);
        if ( !coldef.datadef().range().isNull()) // no range for textdomains
            coldef.datadef().range()->store(_stream);
    }

    if (fcoverage->attributeDefinitions().domain().isValid()){
        std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::interfaceVersion, itDOMAIN,_stream));
        if ( !domainStreamer)
            return false;
         _stream << fcoverage->attributeDefinitions().domain()->valueType();
        storeSystemPath(fcoverage->attributeDefinitions().domain()->resource());
        domainStreamer->store( fcoverage->attributeDefinitions().domain().ptr(), options);

        std::vector<QString> indexes = fcoverage->attributeDefinitions().indexes();
        _stream << (quint32)indexes.size();
        for(auto index : indexes)
            _stream << index;
    }else{
        _stream << itUNKNOWN;
        _stream << QString(sUNDEF);
        _stream << itUNKNOWN;
        _stream << Version::interfaceVersion;
    }


    return true;
}
bool FeatureSerializerV1::storeData(IlwisObject *obj, const IOOptions& options){
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(obj);
    _stream << itFEATURE;
    _stream << Version::interfaceVersion;
    _stream << fcoverage->featureCount();
    for(const SPFeatureI& feature : fcoverage){
        feature->store(fcoverage->attributeDefinitions(),_stream, options);
    }

    return true;
}

bool FeatureSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!CoverageSerializerV1::loadMetaData(obj, options))
        return false;
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(obj);
    int columnCount;
    QString version, url;
    quint64 type;
    IlwisTypes valueType;
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    std::vector<IlwisTypes> types;

    //TODO per level, at the moment only flat featurecoverages for later
    qint32 fcount;
    _stream >> fcount;
    fcoverage->setFeatureCount(itPOINT, fcount,0);
    _stream >> fcount;
    fcoverage->setFeatureCount(itLINE, fcount,0);
    _stream >> fcount;
    fcoverage->setFeatureCount(itPOLYGON, fcount,0);

    _stream >> columnCount;
    for(int col =0; col < columnCount; ++col){
        QString columnName;
        _stream >> columnName;

        _stream >> valueType;
        _stream >> url;
        _stream >> type;
        _stream >> version;
        std::unique_ptr<DataInterface> domainStreamer(factory->create(version, itDOMAIN,_stream));
        if ( !domainStreamer)
            return false;
        IDomain systemDomain = makeSystemObject<IDomain>(url);
        IDomain dom(type | valueType);
        Range *range = 0;
        types.push_back(valueType);
        domainStreamer->loadMetaData(dom.ptr(), options);
        if ( type != itTEXTDOMAIN){
            range = Range::create(dom->valueType());
            if (!range)
                return false;
            range->load(_stream);
        }

        fcoverage->attributeDefinitionsRef().addColumn(ColumnDefinition(columnName, systemDomain.isValid() ? systemDomain : dom));
        if ( range)
            fcoverage->attributeDefinitionsRef().columndefinitionRef(col).datadef().range(range);
    }
    _stream >> valueType;
    _stream >> url;
    _stream >> type;
    _stream >> version;
    if ( type != itUNKNOWN){
        std::unique_ptr<DataInterface> domainStreamer(factory->create(version, itDOMAIN,_stream));
        if ( !domainStreamer)
            return false;
        IDomain systemDomain = makeSystemObject<IDomain>(url);
        IDomain dom(type | valueType);
        domainStreamer->loadMetaData( dom.ptr(), options);
        quint32 nrOfVariants;
        _stream >> nrOfVariants;
        std::vector<QString> variants(nrOfVariants);
        for(int i =0; i < nrOfVariants; ++i){
            _stream >> variants[i];
        }
        fcoverage->attributeDefinitionsRef().setSubDefinition(systemDomain.isValid() ? systemDomain : dom, variants);
    }

    return true;
}

bool FeatureSerializerV1::loadData(IlwisObject* obj, const IOOptions& options ){
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(obj);
    fcoverage->setFeatureCount(itFEATURE, iUNDEF, FeatureInfo::ALLFEATURES); // reset counts as they are set by loadmetadata and are agains et by adding new features
    quint32 featureCount;
    _stream >> featureCount;
    for(quint32 f = 0; f < featureCount; ++f){
        SPFeatureI feature = fcoverage->newFeature(0, false); // create an empty feature
        feature->load(fcoverage->attributeDefinitions(), _stream, options);
    }
    _dataLoaded = true;
    return true;
}
