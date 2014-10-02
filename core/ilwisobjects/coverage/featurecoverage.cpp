#include "kernel.h"
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "table.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "attributetable.h"
#include "feature.h"
#include "featureiterator.h"
#include "geos/geom/CoordinateFilter.h"
#include "geos/geom/PrecisionModel.h"
#ifdef Q_OS_WIN
#include "geos/geom/PrecisionModel.inl"
#endif
#include "geos/geom/GeometryFactory.h"
#include "geos/io/ParseException.h"
#include "geometryhelper.h"
#include "csytransform.h"

using namespace Ilwis;

FeatureCoverage::FeatureCoverage() : _featureTypes(itUNKNOWN),_featureFactory(0)
{
    _featureInfo.resize(4);
    geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel(geos::geom::PrecisionModel::FLOATING);
    _geomfactory.reset(new geos::geom::GeometryFactory(pm,-1));
    delete pm;
}

FeatureCoverage::FeatureCoverage(const Resource& resource) : Coverage(resource),_featureTypes(itUNKNOWN),_featureFactory(0)
{
    _featureInfo.resize(4);
    geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel( geos::geom::PrecisionModel::FLOATING);
    _geomfactory.reset(new geos::geom::GeometryFactory(pm,-1));
    delete pm;
}

FeatureCoverage::~FeatureCoverage() {

}

bool FeatureCoverage::prepare( ) {

    bool ok = Coverage::prepare();
    return ok;
}

std::vector<quint32> FeatureCoverage::select(const QString &spatialQuery) const
{
    ExecutionContext ctx;
    QString expr = QString("script %1=indexes from \"%2\" where %3").arg(Identity::newAnonymousName()).arg(source().url().toString()).arg(spatialQuery);
    Ilwis::SymbolTable tbl;
    if ( Ilwis::commandhandler()->execute(expr, &ctx, tbl) ){
    if ( ctx._results.size() == 1)
        return tbl.getValue<Indices>(ctx._results[0]);
    }
    return std::vector<quint32>();
}

IlwisTypes FeatureCoverage::featureTypes() const
{
    return _featureTypes;
}

void FeatureCoverage::featureTypes(IlwisTypes types)
{
    _featureTypes = types;
}

SPFeatureI FeatureCoverage::newFeature(const QString& wkt, const ICoordinateSystem& foreigncsy, bool load){
    geos::geom::Geometry *geom = GeometryHelper::fromWKT(wkt, foreigncsy.isValid() ? foreigncsy : coordinateSystem());
    if ( !geom)
        throw FeatureCreationError(TR("failed to create feature, is the wkt valid?"));
    return newFeature(geom,load);
}

SPFeatureI FeatureCoverage::newFeature(geos::geom::Geometry *geom, bool load) {

    if ( load) {
        Locker<std::mutex> lock(_loadmutex);
        if (!connector()->dataIsLoaded()) {
            connector()->loadData(this);
        }
    }

    Locker<> lock(_mutex);

    IlwisTypes tp = geometryType(geom);
    auto *newfeature =  createNewFeature(tp);
    if (newfeature ){
        if ( geom) {
            CoordinateSystem *csy = GeometryHelper::getCoordinateSystem(geom);
            if ( csy && !csy->isEqual(coordinateSystem().ptr())){
                CsyTransform trans(csy, coordinateSystem());
                geom->apply_rw(&trans);
            }
            GeometryHelper::setCoordinateSystem(geom, coordinateSystem().ptr());
            newfeature->geometry(geom);
        }
    }
    _features.push_back(newfeature);
    return _features.back();
}

SPFeatureI FeatureCoverage::newFeatureFrom(const SPFeatureI& existingFeature, const ICoordinateSystem& csySource) {
    Locker<> lock(_mutex);

    auto transform = [&](const UPGeometry& geom)->geos::geom::Geometry * {
        geos::geom::Geometry *newgeom = geom->clone();
        if ( csySource.isValid() && !csySource->isEqual(coordinateSystem().ptr())){
            CsyTransform trans(csySource, coordinateSystem());
            newgeom->apply_rw(&trans);
            newgeom->geometryChangedAction();
        }
        GeometryHelper::setCoordinateSystem(newgeom, coordinateSystem().ptr());

        return newgeom;
    };
    if (!connector()->dataIsLoaded()) {
        connector()->loadData(this);
    }
    auto *newfeature = createNewFeature(existingFeature->geometryType());
    const UPGeometry& geom = existingFeature->geometry();
    newfeature->geometry(transform(geom)) ;

    auto variantIndexes = _attributeDefinition.indexes();
    for(auto index : variantIndexes){
        const auto& variant = existingFeature[index];
        auto *variantFeature = createNewFeature(variant->geometryType());
        const auto& geom = variant->geometry();
        variantFeature->geometry(transform(geom)) ;
        newfeature->setSubFeature(index, variantFeature);


    }
    _features.push_back(newfeature);
    return _features.back();
}

FeatureInterface *FeatureCoverage::createNewFeature(IlwisTypes tp) {
    if ( !coordinateSystem().isValid())
        throw FeatureCreationError(TR("No coordinate system set"));

    if ( isReadOnly()){
        throw FeatureCreationError(TR("Readonly feature coverage, no creation allowed"));
    }
    changed(true);

    _featureTypes |= tp;
    if ( _featureFactory == 0) {
        _featureFactory = kernel()->factory<FeatureFactory>("FeatureFactory","ilwis");
    }
    setFeatureCount(tp,1,0);
    CreateFeature create = _featureFactory->getCreator("feature");
    FeatureInterface *newFeature = create(this,0);

    return newFeature;
}


void FeatureCoverage::adaptFeatureCounts(int tp, qint32 featureCnt, quint32 level) {
    if ( featureCnt != iUNDEF){
        _featureInfo[tp]._featureCnt += featureCnt;
        _featureInfo[tp]._featureCnt = std::max(_featureInfo[tp]._featureCnt, 0);
        if ( _featureInfo[tp]._featureCntPerLevel.size() <= level){
            _featureInfo[tp]._featureCntPerLevel.resize(level + 1,0);
        }
        qint32& count = _featureInfo[tp]._featureCntPerLevel[level];
        count += featureCnt;
        count = std::max(count, 0);

    } else{
        if ( level == FeatureInfo::ALLFEATURES){
            _featureInfo[tp]._featureCnt = 0;
            _featureInfo[tp]._featureCntPerLevel.clear();
        }else {
            if ( _featureInfo[tp]._featureCntPerLevel.size() <= level){
                qint32& count = _featureInfo[tp]._featureCntPerLevel[level];
                _featureInfo[tp]._featureCnt -= count;
                count = 0;

            }
        }
    }
}

void FeatureCoverage::setFeatureCount(IlwisTypes types, qint32 featureCnt, quint32 level)
{
    Locker<std::mutex> lock(_mutex2);


    switch(types){
    case itUNKNOWN:
        adaptFeatureCounts(0, featureCnt, level);break;
    case itPOINT:
        adaptFeatureCounts(1, featureCnt, level);break;
    case itLINE:
        adaptFeatureCounts(2, featureCnt, level);break;
    case itPOLYGON:
        adaptFeatureCounts(3, featureCnt, level);break;
    case itFEATURE:
        //usually only used for resetting the count to 0
        adaptFeatureCounts(0, featureCnt, level);
        adaptFeatureCounts(1, featureCnt, level);
        adaptFeatureCounts(2, featureCnt, level);
        adaptFeatureCounts(3, featureCnt, level);
        break;

    }
    if ( featureCnt != iUNDEF) {
        if (featureCnt > 0 )
            _featureTypes |= types;
        else
            _featureTypes &= !types;
    }
}


ITable FeatureCoverage::attributeTable()
{

    // TODO add level parameter to access subfeature attributes

    IAttributeTable tbl;
    tbl.set( new AttributeTable(this));
    return tbl;
}

void FeatureCoverage::attributesFromTable(const ITable& otherTable)
{
    _attributeDefinition.clear();

    for(int col =0; col < otherTable->columnCount(); ++col){
        _attributeDefinition.addColumn(otherTable->columndefinition(col));
    }

    if (otherTable->recordCount() != _features.size())
        return;

    for(int rec =0; rec < otherTable->recordCount(); ++rec){
        auto& feature=  _features[rec];
        feature->record(otherTable->record(rec));
    }
}

FeatureAttributeDefinition &FeatureCoverage::attributeDefinitionsRef(qint32 level)
{
    if ( level <= 0)
        return _attributeDefinition;
    return attributeDefinitionsRef(level - 1);
}

const FeatureAttributeDefinition& FeatureCoverage::attributeDefinitions(qint32 level) const
{
    if ( level <= 0)
        return _attributeDefinition;
    return attributeDefinitions().featureAttributeDefinition(level - 1);
}

IlwisTypes FeatureCoverage::ilwisType() const
{
    return itFEATURE;
}

FeatureCoverage *FeatureCoverage::clone()
{
    FeatureCoverage *fcov = new FeatureCoverage();
    copyTo(fcov);
    return fcov;
}

void FeatureCoverage::copyTo(IlwisObject *obj)
{
    Coverage::copyTo(obj);
    FeatureCoverage *fcov = static_cast<FeatureCoverage *>(obj);
    fcov->_featureTypes = _featureTypes;
    fcov->_featureInfo = _featureInfo;
    fcov->_features.resize(_features.size());
   //fcov->_attributeDefinition = _attributeDefinition;

    for(int i=0; i < _features.size(); ++i){
        if ( _features[i])
            fcov->_features[i].reset(_features[i]->clone(fcov));
    }
}

quint32 FeatureCoverage::featureCount(IlwisTypes types, quint32 level) const
{
    quint32 count=0;
    if ( level == FeatureInfo::ALLFEATURES){
        if ( types == itUNKNOWN)
            count += _featureInfo[0]._featureCnt;
        if ( hasType(types, itPOINT))
            count += _featureInfo[1]._featureCnt;
        if ( hasType(types, itLINE))
            count += _featureInfo[2]._featureCnt;
        if ( hasType(types, itPOLYGON))
            count += _featureInfo[3]._featureCnt;
    }else {

        if ( types == itUNKNOWN){
            if ( level < _featureInfo[0]._featureCntPerLevel.size())
                count += _featureInfo[0]._featureCntPerLevel[level];
        }
        if ( hasType(types, itPOINT)){
            if ( level < _featureInfo[1]._featureCntPerLevel.size())
                count += _featureInfo[1]._featureCntPerLevel[level];
        }
        if ( hasType(types, itLINE)){
            if ( level < _featureInfo[2]._featureCntPerLevel.size())
                count += _featureInfo[2]._featureCntPerLevel[level];
        }
        if ( hasType(types, itPOLYGON)){
            if ( level < _featureInfo[3]._featureCntPerLevel.size())
                count += _featureInfo[3]._featureCntPerLevel[level];
        }
    }
    return count;
}

IlwisTypes FeatureCoverage::geometryType(const geos::geom::Geometry *geom){
    return GeometryHelper::geometryType(geom);
}

const UPGeomFactory& FeatureCoverage::geomfactory() const{
    return _geomfactory;
}
