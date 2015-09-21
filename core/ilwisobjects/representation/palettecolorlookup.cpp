#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlField>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "itemrange.h"
#include "domain.h"
#include "itemdomain.h"
#include "colorrange.h"
#include "numericrange.h"
#include "datadefinition.h"
#include "palettecolorlookup.h"

using namespace Ilwis;
PaletteColorLookUp::PaletteColorLookUp(){

}

PaletteColorLookUp::PaletteColorLookUp(const QString &definition){
    fromDefinition(definition);
}

PaletteColorLookUp::PaletteColorLookUp(const IDomain &dom, const QString &rprCode)
{
    _cyclic = hasType(dom->valueType(), itINDEXEDITEM | itNAMEDITEM | itSTRING);
    InternalDatabaseConnection db;
    QString query = QString("Select * from representation where code='%1'").arg(rprCode != "" ? rprCode : "primarycolors" );
    if (db.exec(query)) {
        if ( db.next()){
            QSqlRecord rec = db.record();
            QString  definition = rec.field("definition").value().toString();
            fromDefinition(definition, dom);
        }
    }
}

PaletteColorLookUp::PaletteColorLookUp(boost::container::flat_map<quint32, QColor> &newcolors) : _colors(newcolors)
{

}

QColor PaletteColorLookUp::value2color(double index, const NumericRange &, const NumericRange &) const
{
    int localIndex  = index;
    if ( _cyclic){
        localIndex = localIndex % _colors.size();
    }
    auto iter = _colors.find(localIndex);
    if ( iter != _colors.end()){

        QColor clr =  (*iter).second;
        return clr;
    }
    return QColor();
}

void PaletteColorLookUp::fromDefinition(const QString &definition, const IDomain& dom){
    QStringList parts = definition.split("|");
    if ( dom.isValid()){
        if ( hasType(dom->valueType(), itTHEMATICITEM|itNUMERICITEM|itTIMEITEM)){
            IItemDomain itemdom = dom.as<ItemDomain<DomainItem>>();
            int index = 0;
            for(auto item : itemdom){
                QString part = parts[index % parts.size()];
                _colors[(quint32)item->raw()] = string2color(part);
                ++index;
            }
            return;
        }
    }
    int index = 0;
    _cyclic = true;
    for( QString part : parts){
        QColor color = string2color(part);
        _colors[index - 1] = color;
        ++index;
    }
}

ColorLookUp *PaletteColorLookUp::clone() const{
    PaletteColorLookUp *newpalette = new PaletteColorLookUp();
    newpalette->_colors = _colors;
    newpalette->_cyclic = _cyclic;
    return newpalette;
}

void PaletteColorLookUp::setColor(double value, const QColor &clr)
{
    _colors[(quint32)value] = clr;
}
