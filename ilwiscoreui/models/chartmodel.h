#ifndef CHARTMODEL_H
#define CHARTMODEL_H

#include <QObject>
#include <QQmlListProperty>
#include "graphmodel.h"
#include "ilwiscoreui_global.h"


class ILWISCOREUISHARED_EXPORT ChartModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList xvalues READ xvalues WRITE xvalues NOTIFY xvaluesChanged)
    Q_PROPERTY(QVariantMap dataset READ dataset NOTIFY datasetChanged)
    Q_PROPERTY(QString xAxis READ xAxis NOTIFY xAxisChanged)
    Q_PROPERTY(QQmlListProperty<GraphModel> graphs READ graphs NOTIFY graphsChanged)
public:
    ChartModel(const QString& xAxisName, const std::vector<QVariant>& xvalues, QObject *parent);
    ChartModel();

    QStringList xvalues() const;
    void xvalues(const std::vector<QVariant>& xvalues);
    void xvalues(const QStringList& xvalues);
    void addGraph(GraphModel * graph);
    void clearGraphs();
    QVariantMap dataset() const;
    QQmlListProperty<GraphModel> graphs();
    QString xAxis() const;


signals:
    void xvaluesChanged();
    void datasetChanged();
    void graphsChanged();
    void xAxisChanged();

private:
    QList<QString> _xvalues;
    QList<GraphModel *> _graphs;
    QString _xAxis;


};

#endif // CHARTMODEL_H
