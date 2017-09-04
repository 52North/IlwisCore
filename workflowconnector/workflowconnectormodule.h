#ifndef WorkflowConnectorMODULE_H
#define WorkflowConnectorMODULE_H

#include <QObject>
#include "module.h"
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"

namespace Ilwis {
namespace WorkflowConnector {

class WorkflowConnectorModule : public Module
{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit WorkflowConnectorModule(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    virtual void finalizePreparation();

    Q_PLUGIN_METADATA(IID "n52.ilwis.workflowconnector" FILE  "workflowConnector.json")

    void prepare();
signals:

public slots:

};
}
}

#endif
