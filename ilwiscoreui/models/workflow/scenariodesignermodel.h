#ifndef SCENARIODESIGNERMODEL_H
#define SCENARIODESIGNERMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

namespace Ilwis{
    class Resource;
}
class WorkflowModel;

class ILWISCOREUISHARED_EXPORT ModelDesigner : public QObject
{
    Q_OBJECT
public:
    explicit ModelDesigner(QObject *parent = 0);
    ~ModelDesigner();

    Q_INVOKABLE WorkflowModel *workflow(const QString& idname);
    Q_INVOKABLE WorkflowModel* addWorkflow(const QString& source);

signals:

public slots:

private:
    QList<WorkflowModel *> _workflows;
};

#endif // SCENARIODESIGNERMODEL_H
