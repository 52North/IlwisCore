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

    Q_PROPERTY(QStringList xvalues READ xvalues WRITE xvalues NOTIFY xvaluesChanged)
    Q_PROPERTY(QList<QVariantMap> datasets READ datasets NOTIFY datasetsChanged)
    Q_PROPERTY(QString xAxis READ xAxis NOTIFY xAxisChanged)
    Q_PROPERTY(QQmlListProperty<GraphModel> graphs READ graphs NOTIFY graphsChanged)
public:
    ChartModel(QObject *parent = 0);

    QStringList xvalues() const;
    void xvalues(const std::vector<QVariant>& xvalues);
    void xvalues(const QStringList& xvalues);
    Q_INVOKABLE void setGraphs(TableModel *tblModel, int type);
    void addGraph(GraphModel * graph);
    void clearGraphs();
    QList<QVariantMap> datasets() const;
    QQmlListProperty<GraphModel> graphs();
    QString xAxis() const;
    void xAxis(const QString& name);
    Q_INVOKABLE void setXAxis(TableModel *tbl, int columnIndex);


signals:
    void xvaluesChanged();
    void datasetsChanged();
    void graphsChanged();
    void xAxisChanged();

private:
    QList<QString> _xvalues;
    QList<GraphModel *> _graphs;
    QString _xAxis;
    int _columnIndex;
    IlwisTypes _valueType = itNUMBER;


};

#endif // CHARTMODEL_H
