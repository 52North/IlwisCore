#ifndef WORKFLOWMODEL_H
#define WORKFLOWMODEL_H

#include <QObject>
#include <QQmlListProperty>

#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT WorkflowModel : public QObject
{
    Q_OBJECT


public:
    explicit WorkflowModel(QObject *parent = 0);

signals:

public slots:

};

typedef QQmlListProperty<WorkflowModel> WorkflowModelList;
Q_DECLARE_METATYPE(WorkflowModelList)

#endif // WORKFLOWMODEL_H
