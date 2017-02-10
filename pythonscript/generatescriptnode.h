#ifndef GENERATESCRIPTNODE_H
#define GENERATESCRIPTNODE_H

namespace Ilwis {
namespace PythonScript{

class GenerateScriptNode
{
public:
    GenerateScriptNode();
    GenerateScriptNode(const SPWorkFlowNode& node);

    std::vector<QString> execute(const OperationExpression& expression, const std::map<quint64, int>& idmap, QStringList& script, const QString &whitespace="");
    void clearCalculatedValues();

private:
    std::vector<QString> _parameterValues;
    SPWorkFlowNode _node;

    std::vector<QString> executeOperation(const OperationExpression& expression, const std::map<quint64, int>& idmap, QStringList& script, const QString& whitespace="");
    std::vector<QString> executeCondition(const OperationExpression& expression, const std::map<quint64, int>& idmap, QStringList& script);
    std::vector<QString> executeJunction(const OperationExpression& expression, const std::map<quint64, int>& idmap, QStringList& script);
    std::vector<QString> executeContent(const OperationExpression &expression, const std::map<quint64, int> &idmap, QStringList& script);
    std::vector<QString> executeLoop(const OperationExpression &expression, const std::map<quint64, int> &idmap, QStringList& script);
    bool next();
    QString parameterValue(int parmIndex) const;
};
}
}

#endif // GENERATESCRIPTNODE_H
