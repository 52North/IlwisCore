#include <QString>
#include <QRegularExpression>
#include <functional>
#include <future>
#include <memory>
#include "geos/geom/PrecisionModel.h"
#include "geos/algorithm/locate/SimplePointInAreaLocator.h"
#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"
#ifdef Q_OS_WIN
#include "geos/geom/PrecisionModel.h"
#include "geos/geom/Envelope.h"
#include "geos/geom/Coordinate.h"
#endif
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/GeometryFactory.h"
#include "coverage.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "raster.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelper.h"
#include "operationhelperfeatures.h"
#include "featureiterator.h"
#include "geometryhelper.h"
#include "selectionbase.h"

using namespace Ilwis;
using namespace BaseOperations;

std::unique_ptr<geos::geom::GeometryFactory> SelectionBase::_geomfactory;

SelectionBase::SelectionBase()
{

}

SelectionBase::SelectionBase(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
    if (!_geomfactory){
        geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel(geos::geom::PrecisionModel::FLOATING);
        _geomfactory.reset(new geos::geom::GeometryFactory(pm,-1));
    }
}

void SelectionBase::ExpressionPart::setEnvelopePolygon(const IRasterCoverage& raster)
{
    double deltax = _envelope.size().xsize() / 10.0;
    double deltay = _envelope.size().ysize() / 10.0;
    std::vector<geos::geom::Coordinate> *coords = new std::vector<geos::geom::Coordinate>();
    double x,y = _envelope.min_corner().y;
    for(x = _envelope.min_corner().x; x < _envelope.max_corner().x; x+= deltax){
       Pixel px = raster->georeference()->coord2Pixel(Coordinate(x,y));
       coords->push_back(geos::geom::Coordinate(px.x, px.y,0));
    }
    for(y = _envelope.min_corner().y; y < _envelope.max_corner().y; y+= deltay){
       Pixel px = raster->georeference()->coord2Pixel(Coordinate(x,y));
       coords->push_back(geos::geom::Coordinate(px.x, px.y,0));
    }
    for(x = _envelope.max_corner().x; x > _envelope.min_corner().x; x-= deltax){
       Pixel px = raster->georeference()->coord2Pixel(Coordinate(x,y));
       coords->push_back(geos::geom::Coordinate(px.x, px.y,0));
    }
    for(y = _envelope.max_corner().y; y > _envelope.min_corner().y; y-= deltay){
       Pixel px = raster->georeference()->coord2Pixel(Coordinate(x,y));
       coords->push_back(geos::geom::Coordinate(px.x, px.y,0));
    }
    Pixel px = raster->georeference()->coord2Pixel(Coordinate(_envelope.min_corner().x,_envelope.min_corner().y));
    coords->push_back(geos::geom::Coordinate(px.x, px.y,0));
    geos::geom::CoordinateArraySequence *points = new  geos::geom::CoordinateArraySequence(coords);
    geos::geom::LinearRing *ring = _geomfactory->createLinearRing(points);
    _polygon.reset( _geomfactory->createPolygon(ring, 0));
}

SelectionBase::ExpressionPart::ExpressionPart(const ICoverage& coverage, const QString& p){
    QString part = p.trimmed();
    _isValid = false;
    int index;
    if ( (index = part.indexOf("envelope("))!= -1) {
        _envelope = QString(part).replace("envelope", "box");
        _isValid = _envelope.isValid();
        _type = ptENVELOPE;
        if ( coverage->ilwisType() == itRASTER && _envelope.isValid()){
            IRasterCoverage raster = coverage.as<RasterCoverage>();
            _box = raster->georeference()->coord2Pixel(_envelope);
            //setEnvelopePolygon(raster);
        }
    } else if ((index = part.indexOf("boundingbox("))==0){
        _box = QString(part).replace("boundingbox", "box");
        _isValid = _box.isValid();
        _type = ptBOX;

    }else if ( (index = part.indexOf("polygon(")) != -1){
        _polygon.reset(GeometryHelper::fromWKT(part,0));
        _isValid = _polygon.get() != 0;
        _type = ptPOLYGON;

    }else if ( part.indexOf("attributes(")!= -1) {
        index = part.indexOf("(");
        int len = part.lastIndexOf(")") - index - 1;
        QString lst = part.mid(index + 1, len);
        QStringList attribs = lst.split(",");
        for(auto attrib : attribs){
            if ( coverage->attributeTable()->columnIndex(attrib) != iUNDEF)    {
                _attributes.push_back(attrib);
                _isValid = true;
            }
        }
        _type = ptATTRIBUTE;
    } else if (part.indexOf("rasterbands(")!=-1){
        index = part.indexOf("(");
        int len = part.lastIndexOf(")") - index - 1;
        QString lst = part.mid(index + 1, len);
        QStringList indexes = lst.split(",");
        IRasterCoverage raster = coverage.as<RasterCoverage>();
        for(auto ind : indexes){
            if (raster->stackDefinition().index(ind) != iUNDEF){
                _bands.push_back(ind);
                _isValid = true;
            }
        }
        _type = ptBANDS;

    }else if ((index = part.indexOf("featuretype("))==0){
        QString ft = part.mid(index + 1);
        if ( ft == "point")
            _geometryType = itPOINT;
        if ( ft == "line")
            _geometryType = itLINE;
        if ( ft == "polygon"){
            _geometryType = itPOLYGON;
        }
        _isValid = true;

    }else {
        std::map<QString,LogicalOperator> operators = {{"==", loEQ},{"<=",loLESSEQ},{">=", loGREATEREQ},{"<",loLESS},{">", loGREATER},{"!=",loNEQ}};
        int index1 = part.indexOf("\"");
        if ( index1 == -1)
            index1 = 10000;
        int index2 = 0;
        for(auto op : operators){
            if ((index2 = part.indexOf(op.first)) != -1){
                if ( index2 < index1)    {
                    _operator = op.second;
                    QString rights = part.mid(index2 + op.first.size()).trimmed();
                    _rightSide = rights.remove("\'");
                    QString leftSide = part.left(index2).trimmed();
                    if ( leftSide.toLower() == "pixelvalue"){
                        _leftSide = iUNDEF;
                        _isValid = true;
                    }else {
                        if ( (_leftSide = coverage->attributeTable()->columnIndex(leftSide)) == iUNDEF){
                            _isValid = false;
                        }else {
                            ColumnDefinition coldef = coverage->attributeTable()->columndefinition(_leftSide);
                            QVariant val = coldef.datadef().domain()->impliedValue(_rightSide);
                            _rightSide = val;
                        }
                    }

                    _type = ptATTRIBUTESELECTION;
                    break;
                }
            }
        }
    }
}


bool SelectionBase::ExpressionPart::match(const SPFeatureI &feature,SelectionBase *operation) const
{
    if ( _type == ExpressionPart::ptFEATURETYPE){
        return hasType(feature->geometryType(), _geometryType);
    }
    if ( _type == ExpressionPart::ptENVELOPE && _envelope.isValid())   {
        return _envelope.contains(feature->geometry().get());
    }
    if ( _type == ExpressionPart::ptPOLYGON && _polygon.get() != 0)   {
        return _polygon->contains(feature->geometry().get());
    }

    if ( _type == ExpressionPart::ptATTRIBUTESELECTION )   {
        QVariant val = feature(_leftSide);
        ColumnDefinition coldef = feature->attributedefinition(_leftSide);
        if ( coldef.datadef().domain()->ilwisType() == itITEMDOMAIN){
            val = coldef.datadef().domain()->impliedValue(val);
        }
        if ( QString(val.typeName()) == "QString" && QString(_rightSide.typeName()) == "QString"){
            return operation->compare1(_operator,val.toString(), _rightSide.toString());
        } else {
            bool ok1,ok2;
            double v1 = val.toDouble(&ok1);
            double v2 = _rightSide.toDouble(&ok2);
            bool ok3 = operation->compare2(_operator, v1, v2);
            return ok1&& ok2 && ok3;
        }
    }
    return false;
}

bool SelectionBase::ExpressionPart::match(const Pixel& location,double pixelValue, SelectionBase *operation) const
{
    if ( _type == ExpressionPart::ptBOX && _box.isValid())   {
        return _box.contains(location);
    }
    if ( _type == ExpressionPart::ptPOLYGON && _polygon.get() != 0)   {
       geos::geom::Point *pnt = operation->pixel2point(location);
       bool ok = _polygon->contains(pnt);
       delete pnt;
       return ok;
    }
    if ( _type == ExpressionPart::ptENVELOPE && _envelope.isValid())   {
        return checkForBounds(location);
    }
    if ( _type == ExpressionPart::ptATTRIBUTESELECTION ) {
        if ( _leftSide != iUNDEF ) { // the pixelvalue pseudo attribute
            bool ok;
            QVariant var = operation->attribute(pixelValue,_leftSide);
            pixelValue = var.toDouble(&ok);
            if (!ok)
                return false;
        }
        bool ok1;
        double v2 = _rightSide.toDouble(&ok1);
        bool ok2 = operation->compare2(_operator, pixelValue, v2);
        return ok1&& ok2;
    }
    return true;
}

bool SelectionBase::ExpressionPart::checkForBounds(const Pixel& location) const{
    bool ok = true;
    if(!_box.contains(location))
        ok = false;
    if ( _polygon){
        geos::geom::Point *pnt = _geomfactory->createPoint(geos::geom::Coordinate(location.x, location.y));
        ok = _polygon->contains(pnt);
        delete pnt;
    }
    return ok;

}

//-------------------------------------------

geos::geom::Point *SelectionBase::pixel2point(const Pixel& pix){
    IRasterCoverage raster = _inputObj.as<RasterCoverage>();
    if ( raster.isValid())    {
        Coordinate crd = raster->georeference()->pixel2Coord(pix);
        return _geomfactory->createPoint(crd);
    }
    return 0;
}

QVariant SelectionBase::attribute(double pixelValue,int columnIndex){
    if ( _inputAttributeTable.isValid()){
       return _inputAttributeTable->cell(columnIndex, pixelValue);
    }
    return false;
}

void SelectionBase::parseSelector(QString selector, const ICoverage& coverage){
    selector.remove('"');

    selector.replace(" with:", " and ");
    QRegularExpression re("( and )|( or )");
    QStringList parts = selector.split(re);

    int lastIndex = 0;
    for(QString& part : parts){
        int index = selector.indexOf(part, lastIndex);
        ExpressionPart epart(coverage, part);
        if ( index != lastIndex){
            QString logical = selector.mid(lastIndex,index - lastIndex);
            logical = logical.trimmed().toLower();
            if ( logical == "and")
                epart._andor = loAND;
            if ( logical == "or")
                epart._andor = loOR;
        }
        lastIndex += part.size();

        _expressionparts.push_back(epart);
    }
}

std::vector<QString> SelectionBase::bands(const IRasterCoverage& raster) const
{
    std::set<QString> bands;
    for(const auto& epart : _expressionparts){
        for(QString bandIndex : epart._bands)
            bands.insert(bandIndex);
        if ( epart._box.isValid()){
            for(quint32 z=epart._box.min_corner().z; z <= epart._box.max_corner().z; ++z){
                bands.insert(raster->stackDefinition().index(z));
            }
        }
    }
    if ( bands.size() == 0){ // no bands selection, so all bands do count
        const RasterStackDefinition& stackdef = raster->stackDefinition();
        for(quint32 z=0; z < stackdef.count(); ++z)
            bands.insert(raster->stackDefinition().index(z));
    }
    std::vector<QString> result(bands.size());
    std::copy(bands.begin(), bands.end(), result.begin());
    return result;
}

QStringList SelectionBase::attributeNames() const
{
    QStringList result;
    for(const auto& epart : _expressionparts){
        result << epart._attributes;
    }
    return result;
}

BoundingBox SelectionBase::boundingBox(const IRasterCoverage& raster) const
{
    BoundingBox box;
    std::set<QString> bands;
    for(const auto& epart : _expressionparts){
        box += epart._box;
        if ( epart._bands.size() != 0){
            for(QString b : epart._bands)
                bands.insert(b) ;
        }
    }
    if ( bands.size() != 0){
        if ( !box.isValid())
            box = raster->size();
        auto &p = box.max_corner();
        p.z = bands.size();
    }
    return box;
}

std::vector<int> SelectionBase::organizeAttributes(){
    std::vector<int> extraAtrrib;
    if ( _inputAttributeTable.isValid()){
        for(const auto& epart : _expressionparts){
            for(int i=0; i < epart._attributes.size();++i){
                int index = _inputAttributeTable->columnIndex(epart._attributes[i]);
                if ( index != iUNDEF){
                    extraAtrrib.push_back(index);
                }
            }
        }
        if ( extraAtrrib.size() == 0){
            for(int c = 0; c < _inputAttributeTable->columnCount(); ++c){
                extraAtrrib.push_back(c);
            }
        }
        if ( _attTable.isValid()){
            for(int c = 0; c < extraAtrrib.size(); ++c){
                _attTable->addColumn( _inputAttributeTable->columndefinition(extraAtrrib[c]));
            }
        }
    }
    return extraAtrrib;
}

