#ifndef INTERNALOPERATIONFACTORY_H
#define INTERNALOPERATIONFACTORY_H

namespace Ilwis {

typedef std::function<IlwisObject *(const Resource & item)> CreateOperation;

namespace Internal {
class InternalOperationFactory : public IlwisObjectFactory
{
public:
    InternalOperationFactory();

    virtual Ilwis::IlwisObject *create(const Resource & item) const;
    virtual IlwisObject *create(IlwisTypes type, const QString &subtype=sUNDEF) const;
    bool canUse(const Ilwis::Resource &item) const;
    quint64 findOperationId(const Ilwis::Resource &item) const;
};


}
}
#endif // INTERNALOPERATIONFACTORY_H
