#ifndef WORKFLOWCATALOGMODEL_H
#define WORKFLOWCATALOGMODEL_H

#include <QObject>
#include <QQmlListProperty>
#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"

#include "catalogmodel.h"
#include "operationmodel.h"
#include "workflowmodel.h"
#include "ilwiscoreui_global.h"

using namespace boost;
typedef adjacency_list<vecS, vecS, directedS> ExecutionBranch;

class WorkflowModel;

class ILWISCOREUISHARED_EXPORT WorkflowCatalogModel : public CatalogModel
{
    Q_OBJECT
    Q_PROPERTY(QMLWorkflowList workflows READ workflows NOTIFY workflowsChanged)

public:

    explicit WorkflowCatalogModel(QObject *parent = 0);

    QMLWorkflowList workflows();
    void nameFilter(const QString&);

private:
    QList<WorkflowModel *> _currentWorkflows;

    void gatherDummyItems();

signals:
    void workflowsChanged();

public slots:

};


#endif // WORKFLOWCATALOGMODEL_H
