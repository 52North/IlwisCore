#ifndef PYTHONWORKFLOWCONNECTOR_H
#define PYTHONWORKFLOWCONNECTOR_H

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
    static ConnectorInterface *create(const Ilwis::Resource &resource,bool load = true,const IOOptions& options=IOOptions());

private:
    std::unique_ptr<QIODevice> _datasource;
    QMap<OVertex, QStringList> _nodeExecutionContext;
    QMap<OVertex, QStringList> _inputArgs;
    OperationExpression _expression;
    QStringList _outputNames;

    bool openTarget();
    bool reverseFollowScriptPath(Workflow *workflow, const OVertex &v, QStringList &names, QStringList &script,int order=-1);
    bool doCondition(const IOperationMetaData& meta, const OVertex &v, QStringList &names);
    bool doInputEdges(InEdgeIterator &ei, const InEdgeIterator &ei_end, Ilwis::Workflow *workflow, const OVertex &v, QStringList &arguments, QStringList &script);
    void executeInputNode(Workflow *workflow, const OVertex &v, QStringList &names, QStringList &script);
    void parseInputNodeArguments(Ilwis::Workflow *workflow, const QList<OVertex> &inputNodes);
    void addGeneratedNames(const OVertex &v, QStringList& names, const Ilwis::IOperationMetaData &meta);
    QString createArgumentList(const Ilwis::IOperationMetaData &meta, const QStringList &arguments);
};
}
}

#endif // PYTHONWORKFLOWCONNECTOR_H
