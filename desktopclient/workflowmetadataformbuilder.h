#ifndef WORKFLOWMETADATAFORMBUILDER_H
#define WORKFLOWMETADATAFORMBUILDER_H


#include <QObject>

namespace Ilwis {
class Resource;
}

class WorkflowMetadataFormBuilder: public QObject
{
    Q_OBJECT
public:
    WorkflowMetadataFormBuilder();
    Q_INVOKABLE QString createWorkflowForm(quint64) const;
};

#endif // WORKFLOWMETADATAFORMBUILDER_H
