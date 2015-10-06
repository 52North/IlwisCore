#include <QColor>
#include <QString>
#include <QSharedPointer>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "itemrange.h"
#include "domainitem.h"
#include "datadefinition.h"
#include "colorrange.h"
#include "coloritem.h"

using namespace Ilwis;

ColorItem::ColorItem(const QColor& clr) : _color(clr)
{
}

IlwisTypes ColorItem::valueType() const
{
    return  ColorItem::valueTypeS();
}

QColor ColorItem::color() const
{
    return _color;
}

void ColorItem::color(const QColor clr)
{
    _color = clr;
}

IlwisTypes ColorItem::valueTypeS()
{
    return itPALETTECOLOR;
}

ItemRange *ColorItem::createRange()
{
    return new ColorPalette();
}

DomainItem *ColorItem::clone() const
{
    return new ColorItem(_color);
}

QString ColorItem::name() const
{
    if(!isValid())
        return sUNDEF;
    return _color.name();
}

bool Ilwis::ColorItem::isValid() const
{
    return _color != clrUNDEF;
}



