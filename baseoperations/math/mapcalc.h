#ifndef MAPCALC_H
#define MAPCALC_H


namespace Ilwis {
namespace BaseOperations{
class MapCalc : public CalculatorOperation
{
public:
    MapCalc();
    MapCalc(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);

    State prepare(ExecutionContext *ctx,const SymbolTable&);

private:
    std::map<int, PixelIterator> _inputRasters;

    IRasterCoverage _outputRaster;

    IDomain findOutDomain(const std::vector<std::vector<QString>>&rpn,const std::vector<QString>& node);
    void fillValues(int pindex, const QString &part, ParmValue &val);
    DataDefinition datadef(int index);
    bool check(int index) const;
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
//class MapCalc7 : public MapCalc{
//public:
//    MapCalc7(quint64 metaid, const Ilwis::OperationExpression &expr);
//    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
//    static quint64 createMetadata();

//    NEW_OPERATION(MapCalc7)    ;
//};
//class MapCalc8 : public MapCalc{
//public:
//    MapCalc8(quint64 metaid, const Ilwis::OperationExpression &expr);
//    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
//    static quint64 createMetadata();

//    NEW_OPERATION(MapCalc8)    ;
//};
//class MapCalc9 : public MapCalc{
//public:
//    MapCalc9(quint64 metaid, const Ilwis::OperationExpression &expr);
//    static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);
//    static quint64 createMetadata();

//    NEW_OPERATION(MapCalc9)    ;
//};
}
}

#endif // MAPCALC_H
