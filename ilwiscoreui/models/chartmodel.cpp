#include "graphmodel.h"
#include "chartmodel.h"

ChartModel::ChartModel(const QString& xAxisName, const std::vector<QVariant> &xvals, QObject *parent) : QObject(parent), _xAxis(xAxisName)
{
    xvalues(xvals);
}

ChartModel::ChartModel()
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

void ChartModel::addGraph(GraphModel *graph)
{
    _graphs.push_back(graph);
}

void ChartModel::clearGraphs()
{
    _graphs.clear();
}

QVariantMap ChartModel::dataset() const
{
    QVariantMap chart;

    chart["labels"] = QVariant::fromValue<QStringList>(_xvalues);
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
    chart["datasets"] = QVariant::fromValue<QList<QVariantMap>>(graphs);

    return chart;
}

QQmlListProperty<GraphModel> ChartModel::graphs()
{
    return QQmlListProperty<GraphModel>(this,_graphs);
}

QString ChartModel::xAxis() const
{
    return _xAxis;
}

