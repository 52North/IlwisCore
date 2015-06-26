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

    OperationModel();
    explicit OperationModel(const Ilwis::Resource &source, QObject *parent=0);
    Q_INVOKABLE explicit OperationModel(quint64 id, QObject *parent=0);

    Q_INVOKABLE QString inputparameterName(quint32 index) const;
    Q_INVOKABLE QString inputparameterType(quint32 index) const;
    Q_INVOKABLE QString inputparameterDescription(quint32 index) const;

    Q_INVOKABLE QString outputparameterName(quint32 index) const;
    Q_INVOKABLE QString outputparameterType(quint32 index) const;
    Q_INVOKABLE QString outputparameterDescription(quint32 index) const;

    Q_INVOKABLE QString syntax() const;
    Q_INVOKABLE QString keywords() const;

    Q_INVOKABLE int maxParameterCount(bool inputCount) const;
    QStringList inParamNames() const;
    QStringList outParamNames() const;


private:
    QVariant property(const QString& name) const;

};

typedef QQmlListProperty<OperationModel> QMLOperationList;

Q_DECLARE_METATYPE(QMLOperationList)

#endif // OPERATIONMODEL_H
