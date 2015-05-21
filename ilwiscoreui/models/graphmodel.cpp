#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "drawers/attributevisualproperties.h"
#include "graphmodel.h"

GraphModel::GraphModel()
{

}

GraphModel::GraphModel(const QString &yAxis, QObject *parent) :
    QObject(parent),
    _fillcolor("transparent"),
    _strokeColor("#009092"),
    _pointColor("transparent"),
    _pointStrokeColor("transparent"),
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
    double sum = 0;
    _yfraction.clear();
    _ycolors.clear();
    for(auto v : data){
        _yfraction.push_back(sum + v.toDouble());
        sum += v.toDouble();
    }
    Ilwis::Geodrawer::VisualAttribute va(Ilwis::IDomain("value"));
    va.actualRange({0,100});
    for(QVariant& v : _yfraction){
        v = 100.0 * v.toDouble() / sum;
        _ycolors.push_back(va.value2color(v));
    }
    _yvalues = data;
}

void GraphModel::yvalues(const std::vector<QVariant> &yvalues)
{
    _yvalues.clear();
    for(auto v : yvalues)    {
        _yvalues.push_back(v.toString());
    }
}

QList<QVariant> GraphModel::yfraction() const
{
    return _yfraction;
}

QList<QVariant> GraphModel::ycolors() const
{
    return _ycolors;
}

QColor GraphModel::ycolor(int index) const
{
    if ( index < _ycolors.size())
        return _ycolors[index].value<QColor>();
    return QColor("grey");
}

bool GraphModel::enabled() const
{
    return _enabled;
}

void GraphModel::enabled(bool yesno)
{
    _enabled = yesno;
    emit enabledChanged();
    emit yAxisChanged();
}

void GraphModel::replaceUndefs(const QString &currentUndef, const QString &newUndef)
{
    double oldUndef, nextUndef;
    if ( currentUndef == "?"){
        oldUndef = Ilwis::rUNDEF;
    }else{
        bool ok;
        oldUndef = currentUndef.toDouble(&ok);
        if ( !ok)
            return;
    }
    bool ok;
    nextUndef = newUndef.toDouble(&ok);

    if (!ok)
        return;
    for(QVariant& value : _yvalues){
        if ( value == oldUndef)
            value = nextUndef;
    }
    emit yAxisChanged();

}

QString GraphModel::yAxis() const
{
    if ( _enabled)
        return _yAxis + " *";
    else
        return _yAxis;
}







