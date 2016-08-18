#ifndef CHARTMODEL_H
#define CHARTMODEL_H

#include <QObject>
#include <QQmlListProperty>
#include "ilwis.h"
#include "graphmodel.h"
#include "ilwiscoreui_global.h"

class TableModel;

class ILWISCOREUISHARED_EXPORT ChartModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<QVariant> xvalues READ xvalues WRITE xvalues NOTIFY xvaluesChanged)
    Q_PROPERTY(QStringList yAttributes READ yAttributes NOTIFY yAttributesChanged)
    Q_PROPERTY(QString xAxis READ xAxis NOTIFY xAxisChanged)
    Q_PROPERTY(int columnIndex READ columnIndex WRITE setColumnIndex NOTIFY columnIndexChanged)
    Q_PROPERTY(QQmlListProperty<GraphModel> graphs READ graphs NOTIFY graphsChanged)
public:
    ChartModel(QObject *parent = 0);
    ChartModel(TableModel *tbl, QObject *p);

    Q_INVOKABLE void setGraphs(int type);
    Q_INVOKABLE GraphModel * graph(int index);
    Q_INVOKABLE QList<QVariant> datasets(int graphType);
    Q_INVOKABLE void setXAxis(int columnIndex);
    Q_INVOKABLE QString xAxis() const;
    Q_INVOKABLE void updateChartWidth(int newWidth);

    QList<QVariant> xvalues();
    void xvalues(const std::vector<QVariant>& xvalues);
    void xvalues(const QList<QVariant>& xvalues);
    void addGraph(GraphModel * graph);
    void clearGraphs();
    QQmlListProperty<GraphModel> graphs();
    void xAxis(const QString& name);
    int columnIndex();
    void setColumnIndex(int ind);
    QStringList yAttributes() const;



signals:
    void xvaluesChanged();
    void graphsChanged();
    void xAxisChanged();
    void columnIndexChanged();
    void yAttributesChanged();

private:
    QList<QVariant> _xvalues;
    QList<GraphModel *> _graphs;
    QString _xAxis;
    int _columnIndex = Ilwis::iUNDEF;
    IlwisTypes _valueTypeXaxis = itNUMBER;
    TableModel *_table;
    int _chartWidth = 0;
    bool _xAxisIsRecordNr = false;
    bool _hasXAggregation = false;


    void fillXValues();
    void fillYValues(QList<QVariant> &yvalues, IlwisTypes valueTypeYAxis, int columnIndex);
};

#endif // CHARTMODEL_H
