#ifndef SIN_H
#define SIN_H

namespace Ilwis {
namespace BaseOperations{
class Sinus : public UnaryMath
{
public:
    Sinus();
    Sinus(quint64 metaid, const Ilwis::OperationExpression &expr);

    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare();

    static quint64 createMetadata();


};
}
}

#endif // SIN_H
