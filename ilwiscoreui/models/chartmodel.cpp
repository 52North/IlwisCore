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

ChartModel::ChartModel(TableModel *tbl, QObject *p) : QObject(p), _table(tbl)
{

}

QList<QVariant> ChartModel::xvalues()
{
   // _xvalues = {"January","February","March","April","May","June","July"};
    return _xvalues;
}

void ChartModel::xvalues(const std::vector<QVariant> &xvalues)
{
    _xvalues.clear();
    for(auto val : xvalues)
        _xvalues.push_back(val.toString());
    emit xvaluesChanged();
}

void ChartModel::xvalues(const QList<QVariant> &xvalues)
{
    _xvalues.clear();
    for(auto val : xvalues)
        _xvalues.push_back(val);
    emit xvaluesChanged();
}

void ChartModel::setGraphs(int type)
{
    if ( _columnIndex == iUNDEF)
        return;

    _graphs.clear();
    if ( hasType(_valueTypeXaxis, itSTRING)) {
        GraphModel *graph = new GraphModel(TR("Histogram"),this);
        std::vector<QVariant> values = _table->table()->column(_columnIndex);
        std::map<QVariant, quint32> counts;
        for(auto v : values){
            counts[v] += 1;
        }
        QList<QVariant> yvalues;
        for(auto p : counts){
            yvalues.push_back(p.second);
        }
        graph->yvalues(yvalues);
        _graphs.push_back(graph);
    }
    emit graphsChanged();
    emit datasetsChanged();
    emit yAttributesChanged();

}

void ChartModel::addGraph(GraphModel *graph)
{
    _graphs.push_back(graph);
}

void ChartModel::clearGraphs()
{
    _graphs.clear();
}

QList<QVariant> ChartModel::datasets() const
{

    QList<QVariant> graphs;
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

int ChartModel::columnIndex()
{
    return _columnIndex;
}

void ChartModel::setColumnIndex(int ind)
{
    if ( ind  >= 0)
        _columnIndex = ind;
}

QStringList ChartModel::yAttributes() const
{
    QStringList lst;

    if ( hasType(_valueTypeXaxis, itSTRING | itITEMDOMAIN) && _hasXAggregation){
        lst.push_back(TR("Histogram"));
    } else {
        for(int i = 0 ; i < _table->table()->columnCount(); ++i){
            const ColumnDefinition &coldef = _table->table()->columndefinitionRef(i);
            IlwisTypes yValueType = coldef.datadef().domain()->valueType();
            if ( hasType(_valueTypeXaxis, itSTRING | itITEMDOMAIN | itNUMBER) &&
                 hasType(yValueType, itSTRING | itITEMDOMAIN | itNUMBER)){
                lst.push_back(coldef.name());
            }
        }
    }
    return lst;
}

void ChartModel::setXAxis(int columnIndex)
{
    if ( columnIndex >= _table->table()->columnCount())
        return;
    _xvalues.clear();
    if ( columnIndex <= 0){
        _valueTypeXaxis = itINT32;
        for(int i =0; i < _table->table()->recordCount(); ++i)
            _xvalues.push_back(i);
    }else {
        columnIndex--; // columnIndex == 0 is the record counter
        std::vector<QVariant> values = _table->table()->column(columnIndex);
        Ilwis::IDomain dom = _table->table()->columndefinition(columnIndex).datadef().domain();
        _valueTypeXaxis = dom->valueType();
        if ( hasType(_valueTypeXaxis, itSTRING)){
            std::set<QString> strings;
            for(auto value : values){
                strings.insert(value.toString());
            }
            _hasXAggregation = values.size() != strings.size();
            for(auto str : strings){
                _xvalues.push_back(QVariant(str));
            }
        }else {
            for(auto value : values){
                _xvalues.push_back(dom->impliedValue(value));
            }
        }
        std::sort(_xvalues.begin(), _xvalues.end());

    }
    _columnIndex = columnIndex;
    _xAxis = _table->table()->columndefinition(columnIndex).name();
    emit yAttributesChanged();


}

