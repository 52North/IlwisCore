#ifndef OPERATIONMODEL_H
#define OPERATIONMODEL_H

#include "resourcemodel.h"
#include <QQmlListProperty>

class ILWISCOREUISHARED_EXPORT OperationModel : public ResourceModel
{
    Q_OBJECT

public:
    Q_PROPERTY(QStringList inParamNames READ inParamNames CONSTANT)
    Q_PROPERTY(QStringList outParamNames READ outParamNames CONSTANT)
    Q_PROPERTY(QString keywords READ keywords CONSTANT)
    Q_PROPERTY(QString syntax READ syntax CONSTANT)
    Q_PROPERTY(QString inParameterCount READ inParameterCount CONSTANT)
    Q_PROPERTY(QString outParameterCount READ outParameterCount CONSTANT)
    Q_PROPERTY(QStringList inParameterIconList READ inParameterIconList CONSTANT)
    Q_PROPERTY(QStringList outParameterIconList READ outParameterIconList CONSTANT)
    Q_PROPERTY(QString provider READ provider CONSTANT)


    OperationModel();
    explicit OperationModel(const Ilwis::Resource &source, QObject *parent=0);
    Q_INVOKABLE explicit OperationModel(quint64 id, QObject *parent=0);

    Q_INVOKABLE QString inputparameterName(quint32 index) const;
    Q_INVOKABLE QString inputparameterTypeNames(quint32 index) const;
    Q_INVOKABLE QString inputparameterType(quint32 index) const;
    Q_INVOKABLE QString inputparameterDescription(quint32 index) const;

    Q_INVOKABLE QString outputparameterName(quint32 index) const;
    Q_INVOKABLE QString outputparameterTypeNames(quint32 index) const;
    Q_INVOKABLE QString outputparameterType(quint32 index) const;
    Q_INVOKABLE QString outputparameterDescription(quint32 index) const;

    QString syntax() const;
    QString keywords() const;
    QString provider() const;

    Q_INVOKABLE int maxParameterCount(bool inputCount) const;

    Q_INVOKABLE QString getProperty(const QString& propertyname) const;
    Q_INVOKABLE bool needChoice(OperationModel *other) const;
    Q_INVOKABLE bool isLegalFlow(OperationModel * from, OperationModel *to) const;
    Q_INVOKABLE QStringList parameterIndexes(const QString &typefilter, bool fromOperation);


    QStringList inParamNames() const;
    QStringList outParamNames() const;
    QString inParameterCount() const;
    QString outParameterCount() const;
    QStringList inParameterIconList() const;
    QStringList outParameterIconList() const;


private:

};

typedef QQmlListProperty<OperationModel> QMLOperationList;

Q_DECLARE_METATYPE(QMLOperationList)

#endif // OPERATIONMODEL_H
