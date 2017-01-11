#ifndef WORKFLOWPARAMETER_H
#define WORKFLOWPARAMETER_H
#include "kernel_global.h"

namespace Ilwis {
class WorkFlowNode;

class KERNELSHARED_EXPORT WorkFlowParameter : public Identity
{
public:
    enum ParameterKind { pkFIXED, pkCALCULATED, pkFREE};

    WorkFlowParameter();
    WorkFlowParameter(int order, int nodeid, const QString& name, const QString& description="");
    QString label() const;
    void label(const QString& lbl);
    void inputLink(const std::shared_ptr<WorkFlowNode> link, qint32 outputIndex = 0);
    std::shared_ptr<WorkFlowNode> inputLink();
    qint32 outputParameterIndex() const;
    QString value() const;
    void value(const QString& v, IlwisTypes type);
    IlwisTypes valueType() const;
    ParameterKind state() const;
    QString syntax() const;
    void addSyntax(const QString& s);
    void attachement(int rectIndex, bool source);
    int attachement(bool source) const;
    void nodeId(int nid);
    int nodeId() const;
    bool isValid() const;

private:
    QString _value = sUNDEF; // value(possible url) of the link or fixed value
    qint32 _outputParameterIndex = iUNDEF; // which parameter to select from the outputs of the link
    std::shared_ptr<WorkFlowNode> _link; // input for this parameter
    IlwisTypes _valueType = itUNKNOWN;
    QString _label;
    ParameterKind _state = pkFREE;
    QString _syntax;
    int _order = iUNDEF;
    int _nodeid = iUNDEF;
    std::pair<int, int> _attachements;
    static quint64 _baseParmId;
};
}

#endif // WORKFLOWPARAMETER_H
