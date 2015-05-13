#include "kernel.h"
#include "graphmodel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "tablemodel.h"
#include "chartmodel.h"

using namespace Ilwis;

ChartModel::ChartModel(QObject *parent) : QObject(parent)
{
}

QStringList ChartModel::xvalues() const
{
    return _xvalues;
}

void ChartModel::xvalues(const std::vector<QVariant> &xvalues)
{
    _xvalues.clear();
    for(auto val : xvalues)
        _xvalues.push_back(val.toString());
    emit xvaluesChanged();
}

void ChartModel::xvalues(const QStringList &xvalues)
{
    _xvalues.clear();
    for(auto val : xvalues)
        _xvalues.push_back(val);
    emit xvaluesChanged();
}

void ChartModel::setGraphs(TableModel *tblModel, int type)
{
    _graphs.clear();
    if ( hasType(_valueType, itSTRING)) {
        GraphModel *graph = new GraphModel(TR("Histogram"),this);
        std::vector<QVariant> values = tblModel->table()->column(_columnIndex);
        std::map<QString, quint32> counts;
        for(auto v : values){
            counts[v.toString()] += 1;
        }
        QList<QVariant> yvalues;
        for(auto p : counts){
            yvalues.push_back(p.second);
        }
        graph->yvalues(yvalues);
        _graphs.push_back(graph);
    }
    emit graphsChanged();
    emit datasetChanged();

}

void ChartModel::addGraph(GraphModel *graph)
{
    _graphs.push_back(graph);
}

void ChartModel::clearGraphs()
{
    _graphs.clear();
}

QList<QVariantMap> ChartModel::datasets() const
{

    QList<QVariantMap> graphs;
    for(auto graph : _graphs){
       QVariantMap graphData;
       graphData["fillColor"] = graph->fillColor();
       graphData["strokeColor"] = graph->strokeColor();
       graphData["pointColor"] = graph->pointColor();
       graphData["pointStrokeColor"] = graph->pointStrokeColor();
       graphData["data"] = graph->yvalues();
       graphs.push_back(graphData);

    }
    return graphs;
}

QQmlListProperty<GraphModel> ChartModel::graphs()
{
    return QQmlListProperty<GraphModel>(this,_graphs);
}

QString ChartModel::xAxis() const
{
    return _xAxis;
}

void ChartModel::xAxis(const QString &name)
{
    _xAxis = name;
}

void ChartModel::setXAxis(TableModel *tbl, int columnIndex)
{
    if ( columnIndex >= tbl->table()->columnCount())
        return;
    std::vector<QVariant> values = tbl->table()->column(columnIndex);
    Ilwis::IDomain dom = tbl->table()->columndefinition(columnIndex).datadef().domain();
    for(auto value : values){
        _xvalues.push_back(dom->impliedValue(value).toString());
    }
    std::sort(_xvalues.begin(), _xvalues.end());
    _columnIndex = columnIndex;
    _xAxis = tbl->table()->columndefinition(columnIndex).name();
    _valueType = dom->valueType();

}

