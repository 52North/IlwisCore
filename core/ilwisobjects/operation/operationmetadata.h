#ifndef OPERATIONMETADATA_H
#define OPERATIONMETADATA_H

#include <QString>
#include <memory>
#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT OperationResource : public Resource{
public:
    enum UIElement{ueNONE=0, ueCOMBO=1, ueLIST=2};
    OperationResource();
    OperationResource(const Resource& resource);
    OperationResource(const QUrl& url, const QString& nmspace="ilwis");
    OperationResource(const QUrl& url, const IlwisTypes &type, const QString& nmspace="ilwis");
    OperationResource(const QUrl& urlNorm, const QUrl& urlRaw, const QString& nmspace="ilwis");
    void setLongName(const QString& longname);
    void setSyntax(const QString& syntax);
    void setInParameterNameFromSyntax(const QString& syntax);
    void setInParameterCount(const std::vector<quint32>& counts);
    void setOutParameterCount(const std::vector<quint32>& counts);
    void addInParameter(quint32 order, IlwisTypes type, const QString& name=sUNDEF, const QString& description=sUNDEF, UIElement altUIType=ueNONE, bool needsQuotes=false);
    void addOptionalInParameter(quint32 order, IlwisTypes type, const QString& name=sUNDEF, const QString& description=sUNDEF, UIElement altUIType=ueNONE);
    void addOutParameter(quint32 order, IlwisTypes type, const QString& name=sUNDEF, const QString& description=sUNDEF);
    void addOptionalOutParameter(quint32 order, IlwisTypes type, const QString& name=sUNDEF, const QString& description=sUNDEF);
    void setKeywords(const QString &words);
    void addValidation(quint32 parent, quint32 child, const QString &validationCondition);
    void parameterNeedsQuotes(quint32 order);
    void setOutputIsInput(int outParm, int inParm);

    bool compatibleOperationAlreadyExists(bool ignoreSelf=true);
};


class KERNELSHARED_EXPORT OperationParameter : public Identity
{
public:
    friend class OperationMetaData;
    enum ParameterKind{ptINPUT, ptOUTPUT};
    OperationParameter(const OperationParameter &operationParameter);
    IlwisTypes type() const;
    OperationParameter::ParameterKind kind() const;
    QString term() const;
    QString domainName() const;
    int linkedInput() const;
    bool needsQuotes() const;
    bool isOptional() const;
    void copyMetaToResourceOf(QScopedPointer<Ilwis::ConnectorInterface> &otherconnector, quint16 index) const;
    void prepare(quint64 base=0);


private:
    OperationParameter(ParameterKind kind, const QString &term, const QString& name, IlwisTypes type, const QString& domain=sUNDEF, const QString &description=sUNDEF, bool optional=false, bool needsQuotes=false, int linkedinput = -1);

    QString _term;
    ParameterKind _kind;
    IlwisTypes _type;
    QString _domainName;
    bool _optional;
    bool _needsQuotes = false;
    int _linkedInput = -1;

    void optional(bool optional);
};

typedef std::shared_ptr<OperationParameter> SPOperationParameter;

class KERNELSHARED_EXPORT OperationMetaData : public IlwisObject
{
public:
    OperationMetaData();
    OperationMetaData(const Resource& resource);
    ~OperationMetaData();
    IlwisTypes ilwisType() const;

    SPOperationParameter newParameter(OperationParameter::ParameterKind kind, const QString &term, const QString& name, IlwisTypes type, const QString& domain=sUNDEF, const QString& description=sUNDEF, bool optional=false, bool needsQuotes=false, int linkedinput=-1);
    SPOperationParameter addParameter(SPOperationParameter parameter);
    std::vector<SPOperationParameter> getInputParameters() const;
    std::vector<SPOperationParameter> getOutputParameters() const;
    quint32 inputParameterCount() const;
    quint32 outputParameterCount() const;
    void parametersFromSyntax(QStringList &required, QStringList &optional);

    QString getNamespace() const;
    QString getLongName() const;
    QStringList getKeywords() const;

    void setNamespace(const QString &nspace);
    void setLongName(const QString &longName);
    void setKeywords(const QStringList &keywords);
    SPOperationParameter inputParameter(int index) const;
    SPOperationParameter outputParameter(int index) const;


    QStringList choiceList(quint32 paramIndex) const;
    bool prepare(const Ilwis::IOOptions &opt=IOOptions());
protected:
    void clearInputParameters();
    void clearOutputParameters();
    void removeParameterProperties(const QString &base, quint16 size);

private:
    quint16 _minInputCountParameters;
    quint16 _minOutputCountParameters;
    std::vector<SPOperationParameter> _inputParameters;
    std::vector<SPOperationParameter> _outputParameters;

    void parmfromResource(int n, const QString &base);

};

typedef Ilwis::IlwisData<Ilwis::OperationMetaData> IOperationMetaData;
}

#endif // OPERATIONMETADATA_H
