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
#include "geos/geom/GeometryFactory.h"
#include "geos/io/ParseException.h"
#include "geometryhelper.h"
#include "csytransform.h"

using namespace Ilwis;

FeatureCoverage::FeatureCoverage() : _featureTypes(itUNKNOWN),_featureFactory(0), _maxIndex(0)
{
    _featureInfo.resize(3);
    _featureInfo[0]._count =  _featureInfo[1]._count =  _featureInfo[2]._count = 0;
    geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel(geos::geom::PrecisionModel::FLOATING);
    _geomfactory.reset(new geos::geom::GeometryFactory(pm,-1));
    delete pm;
}

FeatureCoverage::FeatureCoverage(const Resource& resource) : Coverage(resource),_featureTypes(itUNKNOWN),_featureFactory(0), _maxIndex(0)
{
    _featureInfo.resize(3);
    _featureInfo[0]._count =  _featureInfo[1]._count =  _featureInfo[2]._count = 0;
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

IlwisTypes FeatureCoverage::featureTypes() const
{
    return _featureTypes;
}

void FeatureCoverage::featureTypes(IlwisTypes types)
{
    _featureTypes = types;
}

UPFeatureI& FeatureCoverage::newFeature(const QString& wkt, bool load){
    geos::geom::Geometry *geom = GeometryHelper::fromWKT(wkt.toStdString());
    if ( !geom)
        throw FeatureCreationError(TR("failed to create feature, is the wkt valid?"));
    return newFeature(geom,load);
}

UPFeatureI &FeatureCoverage::newFeature(geos::geom::Geometry *geom, bool load) {

    if ( load) {
        Locker lock(_loadmutex);
        if (!connector()->binaryIsLoaded()) {
            connector()->loadBinaryData(this);
        }
    }

    Locker lock(_mutex);

    UPFeatureI& newfeature = createNewFeature(geometryType(geom));
    if (newfeature ){
        CoordinateSystem *csy = GeometryHelper::getCoordinateSystem(geom);
        if ( csy && !csy->isEqual(coordinateSystem().ptr())){
            CsyTransform trans(csy, coordinateSystem());
            geom->apply_rw(&trans);
        }
        GeometryHelper::setCoordinateSystem(geom, coordinateSystem().ptr());
        newfeature->set(geom);
    }
    return newfeature;
}

UPFeatureI &FeatureCoverage::newFeatureFrom(const UPFeatureI& existingFeature, const ICoordinateSystem& csySource) {
    Locker lock(_mutex);

    if (!connector()->binaryIsLoaded()) {
        connector()->loadBinaryData(this);
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
          newfeature->set(newgeom, i);
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
    IFeatureCoverage fcoverage;
    fcoverage.set(this);
    FeatureInterface *newFeature = create(this);

    qint32 colIndex = fcoverage->attributeTable()->columnIndex(FEATUREIDCOLUMN);
    if ( colIndex == iUNDEF) {
        ERROR1(ERR_NO_INITIALIZED_1, TR("attribute table"));
        throw FeatureCreationError(TR("failed to create feature"));
    }
    newFeature->setCell(colIndex, QVariant(newFeature->featureid()));
    _features.resize(_features.size() + 1);
    _features.back().reset(newFeature);

    quint32 cnt = featureCount(tp);
    setFeatureCount(tp,++cnt );
    return _features.back();
}


void FeatureCoverage::adaptFeatureCounts(int tp, quint32 cnt, int index) {
    auto adapt = [&] () {
        quint32 current =_featureInfo[tp]._perIndex[index];
        qint32 delta = cnt - _featureInfo[tp]._count;
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
    _featureInfo[tp]._count = cnt;
}

void FeatureCoverage::setFeatureCount(IlwisTypes types, quint32 cnt, int index)
{
    Locker lock(_mutex2);
    if (cnt > 0)
        _featureTypes |= types;
    else
        _featureTypes &= !types;

    switch(types){
    case itPOINT:
        adaptFeatureCounts(0, cnt, index);break;
    case itLINE:
        adaptFeatureCounts(1, cnt, index);break;
    case itPOLYGON:
        adaptFeatureCounts(2, cnt, index);break;
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

quint32 FeatureCoverage::featureCount(IlwisTypes types, int index) const
{
    auto countFeatures = [&] (int tp){
        if ( index == iUNDEF)
            return _featureInfo[tp]._count;
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
