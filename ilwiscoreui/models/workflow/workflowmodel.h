#ifndef WORKFLOWMODEL_H
#define WORKFLOWMODEL_H

#include <QObject>
#include <QQmlListProperty>
#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"

#include "operationmodel.h"
#include "ilwiscoreui_global.h"

using namespace boost;

typedef adjacency_list<vecS, vecS, directedS> ExecutionBranch;

class ILWISCOREUISHARED_EXPORT WorkflowModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(WorkflowModelList workflows READ workflows NOTIFY workflowsChanged)

public:

    explicit WorkflowModel(QObject *parent = 0);
    WorkflowModelList workflows();


signals:
    void workflowsChanged();

public slots:

};




class ILWISCOREUISHARED_EXPORT Breakpoint : public QObject
{
    Q_OBJECT

public:
    bool _active;
    //BreakpointProperties
    // OperationModel action(s)

    explicit Breakpoint(QObject *parent = 0);

signals:

public slots:

};



class ILWISCOREUISHARED_EXPORT WorkflowOperation : public QObject
{
    Q_OBJECT


public:
    Breakpoint _breakpoint;
    OperationModel _operation;

    explicit WorkflowOperation(const OperationModel &op , QObject *parent = 0);

signals:

public slots:

};




typedef QQmlListProperty<WorkflowModel> WorkflowModelList;
Q_DECLARE_METATYPE(WorkflowModelList)


#endif // WORKFLOWMODEL_H
