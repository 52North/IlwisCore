#ifndef CALCULATOROPERATION_H
#define CALCULATOROPERATION_H

namespace Ilwis {
namespace BaseOperations{


class CalculatorOperation : public NumericOperation
{
public:
    enum ParmType {NUMERIC, ITERATOR,LINK, STRING, DOMAINITEM,COLUMN};
    enum MathAction{maIFF, maSIN, maCOS, maTAN, maASIN, maACOS,maATAN, maLOG10, maLN, maEXP,
                    maABS,maCEIL, maFLOOR,maSQ,maSQRT,maMAX,maMIN,maPOW,maADD,maMINUS,maDIVIDE,maMULT,
                    maEQ, maNEQ,maLESSEQ,maGREATEREQ,maLESS,maGREATER,maOR, maAND,
                    maUNKNOWN};
    const int LEFT_ASSOC = 0;
    const int RIGHT_ASSOC = 1;




protected:
    struct ParmValue{

        ParmType _type = ITERATOR;
        double _value = rUNDEF;
        int  _link = -1;
        PixelIterator *_source; // for mapcalc
        QString _columName; // for tabcalc
        QString _string; // could be a string value or a colum name
        std::vector<QVariant> _columnValues;
    };
    struct Action{
        std::vector<ParmValue> _values;
        MathAction _action;
    };

    std::map<QString, int> _functions;
    std::map<QString, std::vector<int>> _operators;
    std::map<int, double> _inputNumbers;
    std::map<int,IDomain> _domains;
    std::vector<Action> _actions;
    int _record = 0;

    QStringList tokenizer(const QString &expr);
    CalculatorOperation();
    CalculatorOperation(quint64 metaid, const Ilwis::OperationExpression &expr);
    bool isAssociative(const QString &token, int type) ;
    bool isOperator(const QString &token) ;
    bool isNumber(const QString &token) const;
    bool isFunction(const QString &func);
    int cmpPrecedence(const QString &token1, const QString &token2) ;
    OperationImplementation::State  prepare(ExecutionContext *ctx,const SymbolTable&);
    QStringList shuntingYard(const QString &expr);
    CalculatorOperation::MathAction string2action(const QString &action);
    virtual void fillValues(int pindex, const QString &part, ParmValue &val) = 0;
    virtual bool check(int index) const = 0;
    virtual DataDefinition datadef(int index) = 0;
    IDomain collectDomainInfo(std::vector<std::vector<QString>>& rpn);
    IDomain linearize(const QStringList &tokens);
    double  calc();
    int checkItem(int domainCount, QString &item, QString &copy_item, std::set<QString> &domainItems);
    int checkIndexItem(int domainCount, std::vector<std::vector<QString> > &rpn, std::vector<std::vector<QString> > &copy_rpn, int index, std::set<QString> &domainItems);
    void check(bool ok, const QString& error) const;

private:
    IDomain findOutDomain(const std::vector<std::vector<QString> > &rpn, const std::vector<QString> &node);
};
}
}

#endif // CALCULATOROPERATION_H
