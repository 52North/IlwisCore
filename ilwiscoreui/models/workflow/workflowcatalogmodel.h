#ifndef WORKFLOWCATALOGMODEL_H
#define WORKFLOWCATALOGMODEL_H

#include <QObject>
#include <QQmlListProperty>

#include "../operationcatalogmodel.h"
#include "workflowmodel.h"
#include "ilwiscoreui_global.h"

class WorkflowModel;

class ILWISCOREUISHARED_EXPORT WorkflowCatalogModel : public CatalogModel
{
    Q_OBJECT
    Q_PROPERTY(QMLWorkflowList workflows READ workflows NOTIFY workflowsChanged)

public:

    explicit WorkflowCatalogModel(QObject *parent = 0);

    QMLWorkflowList workflows();
    void nameFilter(const QString&);

    Q_INVOKABLE quint64 workflowId(quint32 index) const;
    Q_INVOKABLE QString executeworkflow(quint64 workflowid, const QString &parameters);

    Q_INVOKABLE  WorkflowModel *newWorkflow(const QString &name);
    Q_INVOKABLE  bool deleteWorkflow(quint32 index);
    Q_INVOKABLE  WorkflowModel *saveWorkflow(quint32 index);

private:
    QList<WorkflowModel *> _currentWorkflows;

signals:
    void error(const QString& err);
    void updateCatalog(const QUrl& url);
    void workflowsChanged();

public slots:

};


#endif // WORKFLOWCATALOGMODEL_H
