#include "graphmodel.h"

GraphModel::GraphModel()
{

}

GraphModel::GraphModel(const QString &yAxis, const std::vector<QVariant> &yvalues, QObject *parent) : QObject(parent), _yAxis(yAxis)
{
    for(auto val : yvalues){
        _yvalues.push_back(val);
    }
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
QString GraphModel::yAxis() const
{
    return _yAxis;
}







