#include "graphmodel.h"

GraphModel::GraphModel()
{

}

GraphModel::GraphModel(const QString &yAxis, QObject *parent) :
    QObject(parent),
    _fillcolor("transparent"),
    _strokeColor("#009092"),
    _pointColor("DarkBlue"),
    _pointStrokeColor("DarkSlateBlue"),
    _yAxis(yAxis)
{

}
QColor GraphModel::fillColor() const
{
    return _fillcolor;
}

void GraphModel::fillColor(const QColor &fillcolor)
{
    _fillcolor = fillcolor;
}
QColor GraphModel::strokeColor() const
{
    return _strokeColor;
}

void GraphModel::strokeColor(const QColor &strokeColor)
{
    _strokeColor = strokeColor;
}
QColor GraphModel::pointColor() const
{
    return _pointColor;
}

void GraphModel::pointColor(const QColor &pointColor)
{
    _pointColor = pointColor;
}
QColor GraphModel::pointStrokeColor() const
{
    return _pointStrokeColor;
}

void GraphModel::pointStrokeColor(const QColor &pointStrokeColor)
{
    _pointStrokeColor = pointStrokeColor;
}
QList<QVariant> GraphModel::yvalues() const
{
    return _yvalues;
}

void GraphModel::yvalues(const QList<QVariant> &data)
{
    _yvalues = data;
}

void GraphModel::yvalues(const std::vector<QVariant> &yvalues)
{
    _yvalues.clear();
    for(auto v : yvalues)    {
        _yvalues.push_back(v.toString());
    }
}

QString GraphModel::yAxis() const
{
    return _yAxis;
}







