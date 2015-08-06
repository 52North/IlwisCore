#ifndef OPERATIONMETADATA_H
#define OPERATIONMETADATA_H

#include <QString>
#include <memory>
#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT OperationResource : public Resource{
public:
    OperationResource(const QUrl& url, const IlwisTypes &type=itSINGLEOPERATION, const QString& nmspace="ilwis");
    void setLongName(const QString& longname);
    void setSyntax(const QString& syntax);
    void setInParameterCount(const std::vector<quint32>& counts);
    void setOutParameterCount(const std::vector<quint32>& counts);
    void addInParameter(quint32 order, IlwisTypes type, const QString& name=sUNDEF, const QString& description=sUNDEF);
    void addOutParameter(quint32 order, IlwisTypes type, const QString& name=sUNDEF, const QString& description=sUNDEF);
    void setKeywords(const QString &words);
};

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
    OperationMetaData(const Resource& resource);
    ~OperationMetaData();
    IlwisTypes ilwisType() const;

    std::unique_ptr<OperationParameter>& newParameter(OperationParameter::ParameterNature kind, const QString& name, IlwisTypes type, const QString& domain=sUNDEF);
    QString npace() const;

private:
    std::vector<std::unique_ptr<OperationParameter>> _inputParameters;
    std::vector<std::unique_ptr<OperationParameter>> _outputParameters;
    QString _namespace;
    QString _longName;
    QStringList _keywords;

    void parmfromResource(const Resource &resource, int n, const QString &base);

};

typedef std::unique_ptr<OperationParameter> UPOperationParameter;
typedef Ilwis::IlwisData<Ilwis::OperationMetaData> IOperationMetaData;
}

#endif // OPERATIONMETADATA_H
