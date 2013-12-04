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
};

class Cosine : public UnaryMath
{
public:
    Cosine() {}
    Cosine(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class Tangent : public UnaryMath
{
public:
    Tangent() {}
    Tangent(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class Arcsine : public UnaryMath
{
public:
    Arcsine() {}
    Arcsine(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class Arccosine : public UnaryMath
{
public:
    Arccosine() {}
    Arccosine(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class ArcTangent : public UnaryMath
{
public:
    ArcTangent() {}
    ArcTangent(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class Log10 : public UnaryMath
{
public:
    Log10() {}
    Log10(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class Sqrt : public UnaryMath
{
public:
    Sqrt() {}
    Sqrt(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class NaturalLogarithm : public UnaryMath
{
public:
    NaturalLogarithm() {}
    NaturalLogarithm(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class Abs : public UnaryMath
{
public:
    Abs() {}
    Abs(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class Ceil : public UnaryMath
{
public:
    Ceil() {}
    Ceil(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class Floor : public UnaryMath
{
public:
    Floor() {}
    Floor(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class Sign : public UnaryMath
{
public:
    Sign() {}
    Sign(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class CosineH : public UnaryMath
{
public:
    CosineH() {}
    CosineH(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};

class SineH : public UnaryMath
{
public:
    SineH() {}
    SineH(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
};
}
}

#endif // UNARYMATHOPERATIONS_H
