#include <QString>
#include <QRegularExpression>
#include <functional>
#include <future>
#include <memory>
#include "geos/geom/Envelope.inl"
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
}

SelectionBase::ExpressionPart::ExpressionPart(const ICoverage& coverage, const QString& part){
    _isValid = false;
    int index;
    if ( (index = part.indexOf("envelope("))!= -1) {
        _envelope = QString(part).replace("envelope", "box");
        _isValid = _envelope.isValid();
        _type = ptENVELOPE;
    } else if ((index = part.indexOf("boundingbox="))==0){
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
            int idx = ind.toInt();
            if ( idx >= 0 && idx < raster->size().zsize())    {
                _bands.push_back(idx);
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
                    if ( (_leftSide = coverage->attributeTable()->columnIndex(leftSide)) == iUNDEF){
                        _isValid = false;
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
    if ( _type == ExpressionPart::ptBOX && _envelope.isValid())   {
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
//-------------------------------------------
void SelectionBase::parseSelector(QString selector, const ITable& attTable){
    selector.remove('"');

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
        }else{
            epart = ExpressionPart(attTable, part);
        }
        lastIndex += part.size();

        _expressionparts.push_back(epart);
    }
}

int SelectionBase::numberOfBandsInSelection() const
{
    int count = 0;
    for(const auto& epart : _expressionparts){
        count += epart._bands.size();
    }
    return count;
}

BoundingBox SelectionBase::boundingBox() const
{
    BoundingBox box;
    for(const auto& epart : _expressionparts){
        box += epart._box;
    }
    return box;
}

