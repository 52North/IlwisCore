#ifndef UNARYMATHOPERATIONS_H
#define UNARYMATHOPERATIONS_H

namespace Ilwis {
namespace BaseOperations{
class Sinus : public UnaryMath
{
public:
    Sinus() {}
    Sinus(quint64 metaid, const Ilwis::OperationExpression &expr);
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
}
}

#endif // UNARYMATHOPERATIONS_H
