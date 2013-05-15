#ifndef OPERATIONMETADATA_H
#define OPERATIONMETADATA_H

#include <QString>
#include <memory>
#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT OperationParameter : public Identity
{
public:
    friend class OperationMetaData;
    enum ParameterNature{ptINPUT, ptOUTPUT};
    IlwisTypes type() const;
    OperationParameter::ParameterNature nature() const;
    quint16 index() const;
    QString domainName() const;

private:
    OperationParameter(quint16 index, ParameterNature kind, const QString& name, IlwisTypes type, const QString& domain=sUNDEF );

    quint16 _index;
    ParameterNature _nature;
    IlwisTypes _type;
    QString _domainName;
};

class KERNELSHARED_EXPORT OperationMetaData : public IlwisObject
{
public:
    OperationMetaData();
    OperationMetaData(const Resource& res);
    ~OperationMetaData();
    IlwisTypes ilwisType() const;

    std::unique_ptr<OperationParameter>& newParameter(OperationParameter::ParameterNature kind, const QString& name, IlwisTypes type, const QString& domain=sUNDEF);
    QString npace() const;

private:
    std::vector<std::unique_ptr<OperationParameter>> _inputParameters;
    std::vector<std::unique_ptr<OperationParameter>> _outputParameters;
    QString _namespace;
    QString _longName;

    void parmfromResource(const Resource &res, int n, const QString &base);

};

typedef std::unique_ptr<OperationParameter> UPOperationParameter;
typedef Ilwis::IlwisData<Ilwis::OperationMetaData> IOperationMetaData;
}

#endif // OPERATIONMETADATA_H
