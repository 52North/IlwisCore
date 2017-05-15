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
    bool _createNewTable;


    Ilwis::IDomain collectDomainInfo(std::vector<std::vector<QString> >& rpn);
    IDomain findOutDomain(const std::vector<std::vector<QString>>&rpn,const std::vector<QString>& node);
    void fillValues(int pindex, const QString &part, ParmValue &val);
    virtual DataDefinition datadef(int index);
    bool check(int index) const;
    void preFetchColumnValues();
};

//--------------------------------------
class TabCalc1 : public TabCalc{
public:
    TabCalc1(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc1)    ;
};

//--------------------------------------
class TabCalc2 : public TabCalc{
public:
    TabCalc2(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc2)    ;
};

//--------------------------------------
class TabCalc3 : public TabCalc{
public:
    TabCalc3(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc3)    ;
};

//--------------------------------------
class TabCalc4 : public TabCalc{
public:
    TabCalc4(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc4)    ;
};

//--------------------------------------
class TabCalc5 : public TabCalc{
public:
    TabCalc5(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc5)    ;
};
//--------------------------------------
class TabCalc6 : public TabCalc{
public:
    TabCalc6(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TabCalc6)    ;
};
}
}

#endif // TABCALC_H
