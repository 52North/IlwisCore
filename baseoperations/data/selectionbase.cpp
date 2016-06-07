#include <QString>
#include <QRegularExpression>
#include <functional>
#include <future>
#include <memory>
#include "geos/geom/PrecisionModel.h"
#include "geos/algorithm/locate/SimplePointInAreaLocator.h"
#include "geos/geom/Point.h"
#ifdef Q_OS_WIN
#include "geos/geom/PrecisionModel.inl"
#include "geos/geom/Envelope.inl"
#endif
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

SelectionBase::SelectionBase()
{

}

SelectionBase::SelectionBase(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
    geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel(geos::geom::PrecisionModel::FLOATING);
    _geomfactory.reset(new geos::geom::GeometryFactory(pm,-1));
}

SelectionBase::ExpressionPart::ExpressionPart(const ICoverage& coverage, const QString& p){
    QString part = p.trimmed();
    _isValid = false;
    int index;
    if ( (index = part.indexOf("envelope("))!= -1) {
        _envelope = QString(part).replace("envelope", "box");
        _isValid = _envelope.isValid();
        _type = ptENVELOPE;
    } else if ((index = part.indexOf("boundingbox("))==0){
        _box = QString(part).replace("boundingbox", "box");
        _isValid = _box.isValid();
        _type = ptBOX;

    }else if ( (index = part.indexOf("polygon(")) != -1){
        _polygon.reset(GeometryHelper::fromWKT(part,0));
        _isValid = _polygon.get() != 0;
        _type = ptPOLYGON;

    }else if ( (index = part.indexOf("attribute="))!= -1) {
        QString lst = part.mid(index + 1);
        QStringList attribs = lst.split(",");
        for(auto attrib : attribs){
            if ( coverage->attributeTable()->columnIndex(attrib) != iUNDEF)    {
                _attributes.push_back(attrib);
                _isValid = true;
            }
        }
        _type = ptATTRIBUTE;
    } else if ((index = part.indexOf("rasterband="))==0){
        QString lst = part.mid(index + 1);
        QStringList indexes = lst.split(",");
        IRasterCoverage raster = coverage.as<RasterCoverage>();
        for(auto ind : indexes){
            if (raster->stackDefinition().index(ind) != iUNDEF){
                _bands.push_back(ind);
                _isValid = true;
            }
        }
        _type = ptBANDS;

    }else if ((index = part.indexOf("featuretype="))==0){
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
                    QString leftSide = part.left(index2).trimmed();
                    if ( leftSide.toLower() == "pixelvalue"){
                        _leftSide = iUNDEF;
                        _isValid = true;
                    }else {
                        if ( (_leftSide = coverage->attributeTable()->columnIndex(leftSide)) == iUNDEF){
                            _isValid = false;
                        }
                    }
                    _rightSide = part.mid(index2 + op.first.size()).trimmed();
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
            bool ok3 = operation->compare1(_operator, v1, v2);
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
    if ( _type == ExpressionPart::ptATTRIBUTESELECTION ) {
        if ( _leftSide == iUNDEF ) { // the pixelvalue pseudo attribute
            bool ok1;
            double v2 = _rightSide.toDouble(&ok1);
            bool ok2 = operation->compare1(_operator, pixelValue, v2);
            return ok1&& ok2;
        }
    }
    return true;
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

void SelectionBase::parseSelector(QString selector, const ITable& attTable){
    selector.remove('"');

    selector.replace(" with:", " and ");
    QRegularExpression re("( and )|( or )");
    QStringList parts = selector.split(re);

    int lastIndex = 0;
    for(QString& part : parts){
        int index = selector.indexOf(part, lastIndex);
        ExpressionPart epart(attTable, part);
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
            for(quint32 z=epart._box.min_corner().z; z < epart._box.max_corner().z; ++z){
                bands.insert(raster->stackDefinition().index(z));
            }
        }
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
    for(const auto& epart : _expressionparts){
        box += epart._box;
        if ( epart._polygon.get() != 0){
            if ( raster.isValid()){
                Envelope env(*(epart._polygon->getEnvelopeInternal()));
                BoundingBox bb = raster->georeference()->coord2Pixel(env);
                box += bb;
            }
        }
    }
    return box;
}

std::vector<int> SelectionBase::organizeAttributes(const ITable& sourceTable){
    std::vector<int> extraAtrrib;
    if ( sourceTable.isValid()){
        for(const auto& epart : _expressionparts){
            if ( epart._type != ExpressionPart::ptATTRIBUTE){
                for(int i=0; i < epart._attributes.size();++i){
                    int index = sourceTable->columnIndex(epart._attributes[i]);
                    if ( index != iUNDEF){
                        extraAtrrib.push_back(index);
                    }
                }
            }
        }
        if ( extraAtrrib.size() == 0){
            for(int c = 0; c < sourceTable->columnCount(); ++c){
                extraAtrrib.push_back(c);
            }
        }
        for(int c = 0; c < extraAtrrib.size(); ++c){
            _attTable->addColumn( sourceTable->columndefinition(extraAtrrib[c]));
        }
    }
    return extraAtrrib;
}

