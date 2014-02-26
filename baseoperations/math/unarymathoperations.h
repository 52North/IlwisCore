#ifndef UNARYMATHOPERATIONS_H
#define UNARYMATHOPERATIONS_H

namespace Ilwis {
namespace BaseOperations{
class Sine : public UnaryMath
{
public:
    Sine() {}
    Sine(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(Sine);
};

class Cosine : public UnaryMath
{
public:
    Cosine() {}
    Cosine(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(Cosine);
};

class Tangent : public UnaryMath
{
public:
    Tangent() {}
    Tangent(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(Tangent);
};

class Arcsine : public UnaryMath
{
public:
    Arcsine() {}
    Arcsine(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(Arcsine);
};

class Arccosine : public UnaryMath
{
public:
    Arccosine() {}
    Arccosine(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();


    NEW_OPERATION(Arccosine);
};

class ArcTangent : public UnaryMath
{
public:
    ArcTangent() {}
    ArcTangent(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(ArcTangent);
};

class Log10 : public UnaryMath
{
public:
    Log10() {}
    Log10(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(Log10);
};

class Sqrt : public UnaryMath
{
public:
    Sqrt() {}
    Sqrt(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(Sqrt);
};

class NaturalLogarithm : public UnaryMath
{
public:
    NaturalLogarithm() {}
    NaturalLogarithm(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();


    NEW_OPERATION(NaturalLogarithm);
};

class Abs : public UnaryMath
{
public:
    Abs() {}
    Abs(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();


    NEW_OPERATION(Abs);
};

class Ceil : public UnaryMath
{
public:
    Ceil() {}
    Ceil(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(Ceil);
};

class Floor : public UnaryMath
{
public:
    Floor() {}
    Floor(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(Floor);
};

class Sign : public UnaryMath
{
public:
    Sign() {}
    Sign(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(Sign);
};

class CosineH : public UnaryMath
{
public:
    CosineH() {}
    CosineH(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(CosineH);
};

class SineH : public UnaryMath
{
public:
    SineH() {}
    SineH(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(SineH);
};
}
}

#endif // UNARYMATHOPERATIONS_H
