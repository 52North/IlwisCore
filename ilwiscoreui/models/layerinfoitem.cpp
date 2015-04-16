#include "layerinfoitem.h"

LayerInfoItem::LayerInfoItem()
{

}

LayerInfoItem::LayerInfoItem(const QString& name, const QString &val, QObject *parent) :
    QObject(parent),
    _name(name),
    _value(val)
{

}

LayerInfoItem::~LayerInfoItem()
{

}

QString LayerInfoItem::name() const
{
    return _name;
}

QString LayerInfoItem::value() const
{
    return _value;
}

QString LayerInfoItem::icon() const
{
    return _icon;
}

void LayerInfoItem::icon(const QString &ic)
{
    _icon = ic;
}

bool LayerInfoItem::layerHeader() const
{
    return _layerHeader;
}

void LayerInfoItem::layerHeader(bool yesno)
{
    _layerHeader = yesno;
}

