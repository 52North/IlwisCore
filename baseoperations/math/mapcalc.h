#ifndef MAPCALC_H
#define MAPCALC_H


namespace Ilwis {
namespace BaseOperations{
class MapCalc : public NumericOperation
{
public:
    MapCalc();
    MapCalc(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);

    State prepare(ExecutionContext *ctx,const SymbolTable&);
private:
    enum ParmType {NUMERIC, ITERATOR,LINK, STRING, DOMAINITEM};
    enum MathAction{maIFF, maSIN, maCOS, maTAN, maASIN, maACOS,maATAN, maLOG10, maLN,
                    maABS,maCEIL, maFLOOR,maSQRT,maMAX,maMIN,maPOW,maADD,maMINUS,maDIVIDE,maMULT,
                    maEQ, maNEQ,maLESSEQ,maGREATEREQ,maLESS,maGREATER,maOR, maAND,
                    maUNKNOWN};
    const int LEFT_ASSOC = 0;
    const int RIGHT_ASSOC = 1;
    struct ParmValue{

        ParmType _type = ITERATOR;
        double _value = rUNDEF;
        int  _link = -1;
        PixelIterator *_iter;
        QString _string; // could be a string value or a colum name
    };
    struct Action{
        std::vector<ParmValue> _values;
        MathAction _action;
    };

    std::map<QString, int> _functions;
    std::map<QString, std::vector<int>> _operators;
    std::map<int, PixelIterator> _inputRasters;
    std::map<int, double> _inputNumbers;
    IRasterCoverage _outputRaster;
    std::map<int,IDomain> _domains;

    bool isAssociative(const QString &token, int type) ;
    bool isOperator(const QString &token) ;
    bool isNumber(const QString &token) const;
    bool isFunction(const QString &func);
    int cmpPrecedence(const QString &token1, const QString &token2) ;
    IDomain linearize(const QStringList &tokens);
    QStringList shuntingYard(const QString &expr);
    std::vector<Action> _actions;

    MapCalc::MathAction string2action(const QString &action);
    double calcBinary(MathAction act, double v1, double v2);
    QStringList tokenizer(const QString &expr);
    Ilwis::IDomain collectDomainInfo(std::vector<std::vector<QString> >& rpn);
    IDomain findOutDomain(const std::vector<std::vector<QString>>&rpn,const std::vector<QString>& node);
};

class MapCalc1 : public MapCalc{
public:
    MapCalc1(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc1)    ;
};

class MapCalc2 : public MapCalc{
public:
    MapCalc2(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc2)    ;
};

class MapCalc3 : public MapCalc{
public:
    MapCalc3(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc3)    ;
};
class MapCalc4 : public MapCalc{
public:
    MapCalc4(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc4)    ;
};

class MapCalc5 : public MapCalc{
public:
    MapCalc5(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc5)    ;
};
class MapCalc6 : public MapCalc{
public:
    MapCalc6(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc6)    ;
};
class MapCalc7 : public MapCalc{
public:
    MapCalc7(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc7)    ;
};
class MapCalc8 : public MapCalc{
public:
    MapCalc8(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc8)    ;
};
class MapCalc9 : public MapCalc{
public:
    MapCalc9(quint64 metaid, const Ilwis::OperationExpression &expr);
    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(MapCalc9)    ;
};
}
}

#endif // MAPCALC_H
