#ifndef OPERATIONMETADATA_H
#define OPERATIONMETADATA_H

#include <QString>
#include <memory>
#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT OperationResource : public Resource{
public:
    OperationResource(const QUrl& url, const QString& nmspace="ilwis");
    OperationResource(const QUrl& url, const IlwisTypes &type, const QString& nmspace="ilwis");
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
    enum ParameterKind{ptINPUT, ptOUTPUT};
    OperationParameter(const OperationParameter &operationParameter);
    IlwisTypes type() const;
    OperationParameter::ParameterKind kind() const;
    quint16 index() const;
    QString domainName() const;
    void index(quint16 index);
    void addToResource(Resource resource) const;

private:
    OperationParameter(ParameterKind kind, const QString& name, IlwisTypes type, const QString& domain=sUNDEF, const QString &description=sUNDEF);

    quint16 _index;
    ParameterKind _kind;
    IlwisTypes _type;
    QString _domainName;
};

typedef std::shared_ptr<OperationParameter> SPOperationParameter;

class KERNELSHARED_EXPORT OperationMetaData : public IlwisObject
{
public:
    OperationMetaData();
    OperationMetaData(const Resource& resource);
    ~OperationMetaData();
    IlwisTypes ilwisType() const;

    SPOperationParameter newParameter(OperationParameter::ParameterKind kind, const QString& name, IlwisTypes type, const QString& domain=sUNDEF, const QString& description=sUNDEF);
    SPOperationParameter newParameter(OperationParameter *parameter);
    SPOperationParameter addParameter(SPOperationParameter parameter);
    std::vector<SPOperationParameter> getInputParameters() const;
    std::vector<SPOperationParameter> getOutputParameters() const;

    QString getNamespace() const;
    QString getLongName() const;
    QStringList getKeywords() const;

    void setNamespace(const QString &nspace);
    void setLongName(const QString &longName);
    void setKeywords(const QStringList &keywords);

protected:
    void clearInputs();
    void clearOutputs();
    quint16 minInputCountParameters();
    quint16 minOutputCountParameters();

private:
    quint16 _minInputCountParameters;
    quint16 _minOutputCountParameters;
    std::vector<SPOperationParameter> _inputParameters;
    std::vector<SPOperationParameter> _outputParameters;

    void parmfromResource(const Resource &resource, int n, const QString &base);

};

typedef Ilwis::IlwisData<Ilwis::OperationMetaData> IOperationMetaData;
}

#endif // OPERATIONMETADATA_H
