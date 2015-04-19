#include "columnmodel.h"

ColumnModel::ColumnModel(const QString &header, quint32 defwidth, QObject *parent) :
    QObject(parent),
    _textColor(QColor("black")),
    _backColor(QColor("lightgrey")),
    _headerText(header),
    _defaultWidth(defwidth)
{

}

ColumnModel::~ColumnModel()
{

}

QString ColumnModel::textColor() const
{
    return _textColor;
}

void ColumnModel::setTextColor(const QColor &clr)
{
    _textColor = clr;
}

QColor ColumnModel::backColor() const
{
    return _backColor;
}

void ColumnModel::setBackColor(const QColor &clr)
{
    _backColor = clr;
}

QString ColumnModel::headerText() const
{
    return _headerText;
}

quint32 ColumnModel::defaultWidth() const
{
    return _defaultWidth;
}

bool ColumnModel::selected() const
{
    return _selected;
}

void ColumnModel::setSelected(bool yesno)
{
    _selected = yesno;
}

