#include "kernel.h"
#include "graphmodel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "tablemodel.h"
#include "chartmodel.h"
#include "mathhelper.h"

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
void ChartModel::fillXValues()
{
   _xvalues.clear();
    NumericRange rng = Ilwis::MathHelper::roundRange(0, _table->table()->recordCount());
    double scale = rng.max() / _chartWidth;
    int marker = rng.min();
    for( int x = 0; x < _chartWidth; ++x){
        double value = x * scale;
        if ( value > marker){
            _xvalues.push_back(QString::number(marker));
            marker += rng.resolution();
        }else
            _xvalues.push_back("");
    }
}

void ChartModel::setXAxis(int columnIndex)
{
    if ( _chartWidth == 0)
        return;

    if ( columnIndex >= _table->table()->columnCount())
        return;
    _xvalues.clear();
    if ( columnIndex <= 0){
        _valueTypeXaxis = itINT32;
        _xAxisIsRecordNr = true;

        fillXValues();
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
            for(auto v : values){
                _xvalues.push_back(dom->impliedValue(v));
            }
        }
    }
    if (!_xAxisIsRecordNr )
        std::sort(_xvalues.begin(), _xvalues.end());

    _columnIndex = columnIndex;
    if ( _columnIndex > 0)
        _xAxis = _table->table()->columndefinition(columnIndex-1).name();
    else
        _xAxis = TR("Record nr");

    emit yAttributesChanged();


}

void ChartModel::fillYValues(QList<QVariant>& yvalues, IlwisTypes valueTypeYAxis, int columnIndex)
{
    QVariant oldValue = 0;
    std::vector<QVariant> values = _table->table()->column(columnIndex);
    NumericRange rng = Ilwis::MathHelper::roundRange(0, _table->table()->recordCount());
    double scale = rng.max() / _chartWidth;
    for( int x = 0; x < _chartWidth; ++x){
        int index = x * scale;
        if ( index < values.size()){

            QVariant v = values[index];
            if ( hasType(valueTypeYAxis,itNUMBER)){
                if ( v == rUNDEF)
                    v = oldValue;
                else
                    oldValue = v;
            }
            yvalues.push_back(v);
        }
    }
}

void ChartModel::setGraphs(int type)
{
    if ( _columnIndex == iUNDEF)
        return ;

    clearGraphs();
    if ( hasType(_valueTypeXaxis, itSTRING|itITEMDOMAIN) && _hasXAggregation) {
        GraphModel *graph = new GraphModel(TR("Histogram") + " " + _xAxis,this);
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
    }else {
        for( int i = 0; i <_table->table()->columnCount(); ++i){
            const ColumnDefinition& coldef = _table->table()->columndefinitionRef(i);
            IlwisTypes valueTypeYAxis = coldef.datadef().domain()->valueType();
            if ( type == 0 || type == 1) {// line && bar
                if ( (hasType(_valueTypeXaxis,itINTEGER) &&_xAxisIsRecordNr) || !_hasXAggregation){
                    if ( hasType(valueTypeYAxis, itNUMBER)){
                        GraphModel *graph = new GraphModel(coldef.name() ,this);
                        QList<QVariant> yvalues;
                        fillYValues(yvalues,valueTypeYAxis,i);
                        graph->yvalues(yvalues);
                        _graphs.push_back(graph);
                    }

                }
            }
        }
    }
    emit graphsChanged();
    emit yAttributesChanged();

}

GraphModel *ChartModel::graph(int index)
{
    if ( index < _graphs.size())    {
        return _graphs[index];
    }
    return 0;
}

void ChartModel::addGraph(GraphModel *graph)
{
    _graphs.push_back(graph);
}

void ChartModel::clearGraphs()
{
    _graphs.clear();
}

QList<QVariant> ChartModel::datasets(int graphType)
{
    if ( _columnIndex == iUNDEF){
        setXAxis(0);
        setGraphs(0);
    }

    QList<QVariant> graphs;
    if ( graphType == 0 || graphType == 1){
        for(auto graph : _graphs){
            if ( graph->enabled()){
                QVariantMap graphData;
                graphData["fillColor"] = graph->fillColor();
                graphData["strokeColor"] = graph->strokeColor();
                graphData["pointColor"] = graph->pointColor();
                graphData["pointStrokeColor"] = graph->pointStrokeColor();
                graphData["data"] = graph->yvalues();
                graphs.push_back(graphData);
            }
        }
    }else if ( graphType == 2 || graphType == 3){
        if ( _graphs.size() == 0)
            return QList<QVariant>();

        auto graph = _graphs[0];
        QList<QVariant> yvalues = graphType == 2 ? graph->yfraction() : graph->yvalues();
        QList<QColor> colors = graph->ycolors();
        for(int index = 0; index < graph->yvalues().size(); ++index){
                QVariantMap graphData;
                graphData["color"] = QVariant(colors[index]);
                graphData["value"] = yvalues[index];
                graphs.push_back(graphData);
        }
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

void ChartModel::updateChartWidth(int newWidth)
{
    _chartWidth = newWidth * 0.8;
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



