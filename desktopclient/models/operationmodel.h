#ifndef OPERATIONMODEL_H
#define OPERATIONMODEL_H

#include "resourcemodel.h"
#include <QQmlListProperty>

class OperationModel : public ResourceModel
{
    Q_OBJECT
public:

    OperationModel();
    explicit OperationModel(const Ilwis::Resource &source, QObject *parent=0);

    Q_INVOKABLE QString inputparameterName(quint32 index) const;
    Q_INVOKABLE QString inputparameterType(quint32 index) const;
    Q_INVOKABLE QString inputparameterDescription(quint32 index) const;

    Q_INVOKABLE QString outputparameterName(quint32 index) const;
    Q_INVOKABLE QString outputparameterType(quint32 index) const;
    Q_INVOKABLE QString outputparameterDescription(quint32 index) const;

    Q_INVOKABLE QString syntax() const;
    Q_INVOKABLE QString keywords() const;

private:
    QVariant property(const QString& name) const;

};

typedef QQmlListProperty<OperationModel> QMLOperationList;

Q_DECLARE_METATYPE(QMLOperationList)

#endif // OPERATIONMODEL_H
