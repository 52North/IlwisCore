#ifndef WORKFLOWPARAMETER_H
#define WORKFLOWPARAMETER_H
#include "kernel_global.h"
#include "location.h"

namespace Ilwis {
class WorkFlowNode;

class KERNELSHARED_EXPORT WorkFlowParameter : public Identity
{
public:
    enum ParameterKind { pkFIXED, pkCALCULATED, pkFREE, pkDONTCARE};

    WorkFlowParameter();
    WorkFlowParameter(int order, int nodeid, const QString& name, const QString& description="");
    QString label() const;
    void label(const QString& lbl);
    QString flowLabel() const;
    void flowLabel( const QString& lbl);
    void inputLink(const std::shared_ptr<WorkFlowNode> link, qint32 outputIndex = 0);
    std::shared_ptr<WorkFlowNode> inputLink();
    const std::shared_ptr<WorkFlowNode> inputLink() const;
    qint32 outputParameterIndex() const;
    QString value() const;
    void value(const QString& v, IlwisTypes type, ParameterKind kind= pkDONTCARE);
    IlwisTypes valueType() const;
    void valueType(IlwisTypes type);
    ParameterKind state() const;
    void state(ParameterKind st);
    QString syntax() const;
    void addSyntax(const QString& s);
    void attachement(int rectIndex, bool source);
    int attachement(bool source) const;
    void nodeId(int nid);
    int nodeId() const;
    qint32 order() const;
    bool isValid() const;
    std::vector<XY> line() const;
    void line(const std::vector<XY>& l);
    bool needsQuotes() const;
    void needsQuotes(bool yesno);


private:
    QString _value = sUNDEF; // value(possible url) of the link or fixed value
    qint32 _outputParameterIndex = iUNDEF; // which parameter to select from the outputs of the link
    std::shared_ptr<WorkFlowNode> _link; // input for this parameter
    IlwisTypes _valueType = itUNKNOWN;
    std::vector<XY> _line;
    QString _label;
    QString _flowLabel;
    ParameterKind _state = pkFREE;
    QString _syntax;
    qint32 _order = iUNDEF;
    int _nodeid = iUNDEF;
    std::pair<int, int> _attachements;
    static quint64 _baseParmId;
    bool _needsQuotes = false;
};


}

#endif // WORKFLOWPARAMETER_H
