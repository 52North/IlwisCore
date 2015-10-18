#ifndef PROJECTIONPARAMETERMODEL_H
#define PROJECTIONPARAMETERMODEL_H

#include <QObject>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "projection.h"
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT ProjectionParameterModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString value READ value CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString valueType READ valueType CONSTANT)
    Q_PROPERTY(int parameterType READ parameterType CONSTANT)
public:
    ProjectionParameterModel();
    explicit ProjectionParameterModel(const Ilwis::IProjection&, Ilwis::Projection::ProjectionParamValue type, QObject *parent = 0);
    ~ProjectionParameterModel();

    QString name() const;
    QString value() const;
    QString description() const;
    QString valueType() const;
    int parameterType() const;


signals:

public slots:

private:
    QString _name;
    QString _value;
    QString _description;
    IlwisTypes _valueType;
    int _parmType = Ilwis::Projection::pvNONE;
};

#endif // PROJECTIONPARAMETERMODEL_H
