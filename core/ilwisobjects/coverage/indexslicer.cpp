#include "kernel.h"
#include "raster.h"
#include "rastercoverage.h"
#include "pixeliterator.h"
#include "connectorinterface.h"
#include "symboltable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "numericdomain.h"
#include "numericitem.h"
#include "numericitemrange.h"
#include "ilwisoperation.h"
#include "indexslicer.h"

using namespace Ilwis;

IndexSlicer::IndexSlicer(const IRasterCoverage &cov) : _raster(cov)
{
}

void IndexSlicer::grid(const IRasterCoverage &cov)
{
    _raster = cov;
}

IRasterCoverage IndexSlicer::operator()(const QString &item)
{
    IDomain indexDomain = _raster->datadef().domain(DataDefinition::daLAYERINDEX);
    if (!indexDomain.isValid())
        return IRasterCoverage();
    if ( hasType(indexDomain->valueType(), itNUMERIC)) {

        QString basename = makeBaseName();

        double index;
        if ( indexDomain->valueType() == itNUMERICITEM)  {
            index = _raster->layerIndex(item);
        } else if ( hasType(indexDomain->valueType(),itNUMBER)) {
            index  = findIndexNumber(indexDomain, item.toDouble());
        }
        if ( index == rUNDEF){
            ERROR2(ERR_INVALID_PROPERTY_FOR_2, TR("item boundary"), TR("Index slicing"));
            return IRasterCoverage();
        }
        QString cname;
        QString expr = makeExpression(index, basename, cname);

        IRasterCoverage mp = Operation::calculate<IRasterCoverage>(cname,expr);
        if ( mp.isValid())
            return mp;
    }
    return IRasterCoverage();
}

QString IndexSlicer::makeBaseName() const {
    QString basename = _raster->name();
    int ind = 0;
    if( (ind=basename.lastIndexOf(".")) != -1){
        basename = basename.left(ind);
    }
    return basename;
}

QString IndexSlicer::makeExpression(double index, const QString& basename, QString& cname) {
    double rest1 = index - (int)index;
    QString expr =  _raster->datadef().range(DataDefinition::daLAYERINDEX)->interpolation();
    bool isContinous = _raster->datadef().range(DataDefinition::daLAYERINDEX)->isContinuous();
    if ( std::abs(rest1) > EPS8 && isContinous) {
        int lowerIndex = std::floor(index);
        double rest2 = 1.0 - rest1;
        cname = QString("%1_%2_%3").arg(basename).arg(lowerIndex).arg(lowerIndex+1);
        expr = cname + "=" + expr.arg(QString("%1*%2[%3]")
                                      .arg(rest2)
                                      .arg(_raster->name())
                                      .arg(lowerIndex))
                .arg(QString("%1*%2[%3]").
                     arg(rest1).
                     arg(_raster->name()).
                     arg(lowerIndex+1));
    } else {
        cname = QString("%1_%2").arg(basename).arg((int)index);
        expr = QString("%1=%2[%3]").arg(cname).arg( _raster->name()).arg((int)index);

    }
    return expr;
}

double IndexSlicer::findIndexNumber(const IDomain& indexDomain, double itemIndex) const{
    INumericDomain numdom = indexDomain.get<NumericDomain>();
    SPNumericRange numrange = numdom->range<NumericRange>();
    if ( numrange->contains(itemIndex))
        return itemIndex;
    return rUNDEF;
}

double IndexSlicer::findIndexNumericalItem(const IDomain& indexDomain, const QString& itemIndex) const{
//    INumericItemDomain numdom = indexDomain.get<NumericItemDomain>();
//    SPNumericItemRange numrange = numdom->range<NumericItemRange>();
//    //double index1 = _raster->index(indexItem);
//    double index1 = numrange->index(itemIndex);

//    return index1;
}
