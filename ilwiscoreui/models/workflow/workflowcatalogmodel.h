#ifndef WORKFLOWCATALOGMODEL_H
#define WORKFLOWCATALOGMODEL_H

#include <QObject>
#include <QQmlListProperty>

#include "operationcatalogmodel.h"
#include "workflowmodel.h"
#include "ilwiscoreui_global.h"

class WorkflowModel;

class ILWISCOREUISHARED_EXPORT WorkflowCatalogModel : public CatalogModel
{
    Q_OBJECT
    Q_PROPERTY(QMLWorkflowList workflows READ workflows NOTIFY workflowsChanged)
    Q_PROPERTY(QString currentWorkflow READ currentWorkflow WRITE setCurrentWorkflow NOTIFY currentWorkflowChanged)

public:

    explicit WorkflowCatalogModel(QObject *parent = 0);

    QMLWorkflowList workflows();
    void nameFilter(const QString&);
    QString currentWorkflow() const;
    void setCurrentWorkflow(const QString &workflowName);

    Q_INVOKABLE quint64 workflowId(quint32 index) const;
    Q_INVOKABLE QString executeworkflow(quint64 workflowid, const QString &parameters);
    Q_INVOKABLE bool hasActiveEditSession() const;

    Q_INVOKABLE WorkflowModel *newWorkflow(const QString &name, const QString &description="");
    Q_INVOKABLE WorkflowModel *saveWorkflow(quint32 index);
    Q_INVOKABLE bool deleteWorkflow(quint32 index);


private:
    QList<WorkflowModel *> _currentWorkflows;
    QString _currentWorkflow;

signals:
    void error(const QString& err);
    void updateCatalog(const QUrl& url);
    void workflowsChanged();
    void currentWorkflowChanged();

public slots:

};


#endif // WORKFLOWCATALOGMODEL_H
