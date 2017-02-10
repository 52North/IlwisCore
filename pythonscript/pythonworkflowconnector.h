#ifndef PYTHONWORKFLOWCONNECTOR_H
#define PYTHONWORKFLOWCONNECTOR_H

#include <QBuffer>

namespace Ilwis{
class Workflow;

namespace PythonScript {
class PythonWorkflowConnector : public Ilwis::IlwisObjectConnector
{
public:
    PythonWorkflowConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool store(IlwisObject *, const IOOptions& options = IOOptions() );
    QString type() const;
    Ilwis::IlwisObject *create() const;
    QString provider() const;
    QVariant getProperty(const QString &name) const;

    static ConnectorInterface *create(const Ilwis::Resource &resource,bool load = true,const IOOptions& options=IOOptions());
    static ConnectorInterface *create2(const Ilwis::Resource &resource, bool load=true, const IOOptions &options=IOOptions());

private:
    std::unique_ptr<QIODevice> _datasource;
    OperationExpression _expression;
    QStringList _outputNames;
    QByteArray _data;

    bool openTarget();
};
}
}

#endif // PYTHONWORKFLOWCONNECTOR_H
