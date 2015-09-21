#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlField>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "numericrange.h"
#include "continuouscolorlookup.h"

using namespace Ilwis;



ContinuousColorLookup::ContinuousColorLookup()
{

}

ContinuousColorLookup::ContinuousColorLookup(const QString &definition)
{
    fromDefinition(definition);
}

ContinuousColorLookup::ContinuousColorLookup(const IDomain &, const QString& rprCode)
{
    InternalDatabaseConnection db;
    QString query = QString("Select * from representation where code='%1'").arg(rprCode != "" ? rprCode : "pseudo");
    if (db.exec(query)) {
        if ( db.next()){
            QSqlRecord rec = db.record();
            QString  definition = rec.field("definition").value().toString();
            fromDefinition(definition);
        }
    }


}

QColor ContinuousColorLookup::value2color(double value, const NumericRange& actualRange, const NumericRange& stretchRange) const
{
    if ( value == rUNDEF)
        return QColor("transparent");

    if ( stretchRange.isValid())  {
        if ( _linear){
            if ( value < stretchRange.center()){
                double stretchFraction = (value - stretchRange.min())/ stretchRange.distance();
                value = actualRange.min() + stretchFraction * actualRange.distance();
            }else{
                if ( value >= stretchRange.center()){
                    double stretchFraction = (stretchRange.max() - value)/ stretchRange.distance();
                    value = actualRange.max() - stretchFraction * actualRange.distance();
                }
            }
        }
    }
    value = (value - actualRange.min()) / actualRange.distance(); // scale it between 0..1
    for(int i = 0; i < _groups.size(); ++i){
        if ( value <= _groups[i].max()){
            double delta = _groups[i].distance();
            double position = 0;
            if ( _step == 0){
                position = (value - _groups[i].min())/ delta;
            }else
                position = ((quint32)(value - _groups[i].min())/ _step)/( (quint32)(delta / _step));

            return ContinuousColorRange::valueAt(position,&_colorranges[i]);
        }
    }
    return QColor();
}

void ContinuousColorLookup::addGroup(const NumericRange &range, const ContinuousColorRange &colorrange)
{
    if ( !(range.min() >= 0 && range.max() <= 1.0)){
        ERROR2(ERR_INVALID_INIT_FOR_2, TR("Numerical range"), "Representation");
        return;
    }
    if ( _colorranges.size() > 0){
        if ( range.min() < _groups.back().min()){
            kernel()->issues()->log(TR("Numerical ranges for representation must be added in order"));
            return ;
        }
    }
    _colorranges.push_back(colorrange);
    _groups.push_back(range);
}

void ContinuousColorLookup::setColor(double value, const QColor &clr)
{
    // TODO:
}

ColorLookUp *ContinuousColorLookup::clone() const
{
    ContinuousColorLookup *newlookup = new ContinuousColorLookup();
    newlookup->_colorranges = _colorranges;
    newlookup->_groups = _groups;
    newlookup->_linear = _linear;
    newlookup->_numericRange = _numericRange;
    newlookup->_step = _step;

    return newlookup;
}

void ContinuousColorLookup::fromDefinition(const QString &definition)
{
    QStringList parts = definition.split(";");
    for( QString group : parts){
        QStringList groupdef = group.split("|");
        if ( groupdef.size() != 3) {
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        QStringList limits = groupdef[0].split(":");
        if ( limits.size() != 2){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        bool ok1, ok2;
        NumericRange numrange(limits[0].toDouble(&ok1), limits[1].toDouble(&ok2));
        if ( !(ok1 && ok2)){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        QColor color1 = string2color(groupdef[1]);
        if ( !(color1.isValid())){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }


        QColor color2 = string2color(groupdef[2]);
        if ( !(color2.isValid())){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        ContinuousColorRange colorrange(color1, color2);
        addGroup(numrange,colorrange);

    }

}
