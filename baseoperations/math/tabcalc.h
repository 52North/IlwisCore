#ifndef TABCALC_H
#define TABCALC_H


namespace Ilwis {
namespace BaseOperations{
class TabCalc : public CalculatorOperation
{
public:
    TabCalc();
    TabCalc(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);

    State prepare(ExecutionContext *ctx,const SymbolTable&);
private:
    std::map<int, QString> _inputColumns;
    ITable _inputTable;
    ITable _outputTable;
    QString _outputColumn;
    bool _copyTable;


    Ilwis::IDomain collectDomainInfo(std::vector<std::vector<QString> >& rpn);
    IDomain findOutDomain(const std::vector<std::vector<QString>>&rpn,const std::vector<QString>& node);
    void fillValues(int pindex, const QString &part, ParmValue &val);
    bool check(int index) const;
};

//--------------------------------------
class TabCalc1 : publicTabCalc{
public:
    TabCalc1(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc1)    ;
};
}
}

#endif // TABCALC_H
