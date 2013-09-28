#ifndef LOGICALEXPRESSIONPARSER_H
#define LOGICALEXPRESSIONPARSER_H

namespace Ilwis {

class LogicalExpressionPart {
public:
    LogicalExpressionPart(const QString& part);
    QString field() const;
    LogicalOperator condition() const;
    QString value() const;
    IlwisTypes valueType() const;
    LogicalOperator logicalConnector() const;
    void logicalConnector(LogicalOperator oper);
    bool isValid() const;
private:
    QString _field = sUNDEF;
    LogicalOperator _condition = loNONE;
    QString _value = sUNDEF;
    IlwisTypes _vt = itUNKNOWN;
    LogicalOperator _logicalOper = loNONE;


};

class LogicalExpressionParser
{
public:

    LogicalExpressionParser();
    LogicalExpressionParser(const QString& expr);
    bool parse(const QString& expr);
    bool isValid() const;
    std::vector<LogicalExpressionPart> parts() const;
private:
    std::vector<LogicalExpressionPart> _parts;


};
}

#endif // LOGICALEXPRESSIONPARSER_H
