#include "kernel.h"
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
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

FeatureCoverage::FeatureCoverage() : _featureTypes(itUNKNOWN),_featureFactory(0), _maxIndex(0)
{
    _featureInfo.resize(3);
    geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel(geos::geom::PrecisionModel::FLOATING);
    _geomfactory.reset(new geos::geom::GeometryFactory(pm,-1));
    delete pm;
}

FeatureCoverage::FeatureCoverage(const Resource& resource) : Coverage(resource),_featureTypes(itUNKNOWN),_featureFactory(0), _maxIndex(0)
{
    _featureInfo.resize(3);
    geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel( geos::geom::PrecisionModel::FLOATING);
    _geomfactory.reset(new geos::geom::GeometryFactory(pm,-1));
    delete pm;
}

FeatureCoverage::~FeatureCoverage() {

}

bool FeatureCoverage::prepare( ) {

    bool ok = Coverage::prepare();
    if ( ok && !attributeTable().isValid()){
        if ( isAnonymous())
            ok =  attributeTable().prepare();
        else
            ok =  attributeTable().prepare(name());

    }
    if ( ok && attributeTable()->columnIndex(FEATUREIDCOLUMN) == iUNDEF)
        attributeTable()->addColumn(FEATUREIDCOLUMN,"count");
    return ok;
}

Indices FeatureCoverage::select(const QString &spatialQuery) const
{
    ExecutionContext ctx;
    QString expr = QString("script %1=indexes from \"%2\" where %3").arg(Identity::newAnonymousName()).arg(source().url().toString()).arg(spatialQuery);
    Ilwis::SymbolTable tbl;
    if ( Ilwis::commandhandler()->execute(expr, &ctx, tbl) ){
    if ( ctx._results.size() == 1)
        return tbl.getValue<Indices>(ctx._results[0]);
    }
    return Indices();
}

IlwisTypes FeatureCoverage::featureTypes() const
{
    return _featureTypes;
}

void FeatureCoverage::featureTypes(IlwisTypes types)
{
    _featureTypes = types;
}

UPFeatureI& FeatureCoverage::newFeature(const QString& wkt, const ICoordinateSystem& foreigncsy, bool load){
    geos::geom::Geometry *geom = GeometryHelper::fromWKT(wkt, foreigncsy.isValid() ? foreigncsy : coordinateSystem());
    if ( !geom)
        throw FeatureCreationError(TR("failed to create feature, is the wkt valid?"));
    return newFeature(geom,load);
}

UPFeatureI &FeatureCoverage::newFeature(geos::geom::Geometry *geom, bool load) {

    if ( load) {
        Locker lock(_loadmutex);
        if (!connector()->dataIsLoaded()) {
            connector()->loadData(this);
        }
    }

    Locker lock(_mutex);

    IlwisTypes tp = geometryType(geom);
    UPFeatureI& newfeature = createNewFeature(tp);
    if (newfeature ){
        if ( geom) {
            CoordinateSystem *csy = GeometryHelper::getCoordinateSystem(geom);
            if ( csy && !csy->isEqual(coordinateSystem().ptr())){
                CsyTransform trans(csy, coordinateSystem());
                geom->apply_rw(&trans);
            }
            GeometryHelper::setCoordinateSystem(geom, coordinateSystem().ptr());
            newfeature->add(geom);
        }
    }
    return newfeature;
}

UPFeatureI &FeatureCoverage::newFeatureFrom(const UPFeatureI& existingFeature, const ICoordinateSystem& csySource) {
    Locker lock(_mutex);

    if (!connector()->dataIsLoaded()) {
        connector()->loadData(this);
    }
    UPFeatureI& newfeature = createNewFeature(existingFeature->geometryType());
    if (newfeature == nullptr)
        return newfeature;

    for(int i=0; i < existingFeature->trackSize(); ++i){
          UPGeometry& geom = existingFeature->geometry(i);
          geos::geom::Geometry *newgeom = geom->clone();
          if ( csySource.isValid() && csySource->isEqual(coordinateSystem().ptr())){
              CsyTransform trans(csySource, coordinateSystem());
              newgeom->apply_rw(&trans);
          }
          GeometryHelper::setCoordinateSystem(newgeom, coordinateSystem().ptr());
          newfeature->add(newgeom, existingFeature->trackIndexValue(i));
          //setFeatureCount(newfeature->geometryType(),i, newgeom->getNumGeometries() );
    }
    return newfeature;
}

Ilwis::UPFeatureI &FeatureCoverage::createNewFeature(IlwisTypes tp) {
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
    //_record.reset(new AttributeRecord(attributeTable(),FEATUREIDCOLUMN ));

    CreateFeature create = _featureFactory->getCreator("feature");
    FeatureInterface *newFeature = create(this);

    qint32 colIndex = this->attributeTable()->columnIndex(FEATUREIDCOLUMN);
    if ( colIndex == iUNDEF) {
        ERROR1(ERR_NO_INITIALIZED_1, TR("attribute table"));
        throw FeatureCreationError(TR("failed to create feature"));
    }
    newFeature->setCell(colIndex, QVariant(newFeature->featureid()));
    _features.resize(_features.size() + 1);
    _features.back().reset(newFeature);


    return _features.back();
}


void FeatureCoverage::adaptFeatureCounts(int tp, quint32 geomCnt, quint32 subGeomCnt, int index) {
    auto adapt = [&] () {
        quint32 current =_featureInfo[tp]._perIndex[index];
        qint32 delta = geomCnt - _featureInfo[tp]._geomCnt;
        _featureInfo[tp]._perIndex[index] = current + delta;
    };

    if ( index < _featureInfo[tp]._perIndex.size()){
        adapt();
    } else {
        if ( index >= _featureInfo[tp]._perIndex.size() ) {
            _featureInfo[tp]._perIndex.resize(index + 1,0);
            _maxIndex = index + 1;
        }
        adapt();
    }
    _featureInfo[tp]._geomCnt += geomCnt;
    if ( geomCnt != 0)
        _featureInfo[tp]._subGeomCnt += geomCnt > 0 ? subGeomCnt : -subGeomCnt;
    else {
        _featureInfo[tp]._subGeomCnt = 0;
        _featureInfo[tp]._geomCnt = 0;
    }
}

void FeatureCoverage::setFeatureCount(IlwisTypes types, quint32 geomCnt, quint32 subGeomCnt, int index)
{
    Locker lock(_mutex2);
    if (geomCnt > 0)
        _featureTypes |= types;
    else
        _featureTypes &= !types;

    switch(types){
    case itPOINT:
        adaptFeatureCounts(0, geomCnt, subGeomCnt, index);break;
    case itLINE:
        adaptFeatureCounts(1, geomCnt, subGeomCnt, index);break;
    case itPOLYGON:
        adaptFeatureCounts(2, geomCnt, subGeomCnt, index);break;
    case itFEATURE:
        //usually only used for resetting the count to 0
        adaptFeatureCounts(0, geomCnt, subGeomCnt, index);
        adaptFeatureCounts(1, geomCnt, subGeomCnt, index);
        adaptFeatureCounts(2, geomCnt, subGeomCnt, index);
        break;

    }
}

quint32 FeatureCoverage::maxIndex() const
{
    return _maxIndex;
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

    for(int i=0; i < _features.size(); ++i){
        if ( _features[i])
            fcov->_features[i].reset(_features[i]->clone());
    }
}

quint32 FeatureCoverage::featureCount(IlwisTypes types, bool subAsDistinct, int index) const
{
    auto countFeatures = [&] (int tp){
        if ( index == iUNDEF){
            if (subAsDistinct)
                return _featureInfo[tp]._subGeomCnt;
            return _featureInfo[tp]._geomCnt;
        }
        else
            return  _featureInfo[tp]._perIndex.size() < index ? _featureInfo[tp]._perIndex[index] : 0;
    };

    quint32 count=0;
    if ( hasType(types, itPOINT))
        count += countFeatures(0);
    if ( hasType(types, itLINE))
        count += countFeatures(1);
    if ( hasType(types, itPOLYGON))
        count += countFeatures(2);

    return count;
}

IlwisTypes FeatureCoverage::geometryType(const geos::geom::Geometry *geom){
    return GeometryHelper::geometryType(geom);
}

const UPGeomFactory& FeatureCoverage::geomfactory() const{
    return _geomfactory;
}
