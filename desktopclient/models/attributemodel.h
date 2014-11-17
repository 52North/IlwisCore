#ifndef ATTRIBUTEMODEL_H
#define ATTRIBUTEMODEL_H

#include <QObject>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"

class AttributeModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString attributename READ attributename CONSTANT)
    Q_PROPERTY(QString defaultRangeDefinition READ defaultRangeDefinition CONSTANT)
    Q_PROPERTY(QString actualRangeDefintion READ actualRangeDefintion CONSTANT)
    Q_PROPERTY(QString attributeDomain READ attributeDomain CONSTANT)
    Q_PROPERTY(QString attributeValueType READ attributeValueType CONSTANT)

public:
    AttributeModel();
    explicit AttributeModel(const Ilwis::ColumnDefinition& def, QObject *parent);

    QString defaultRangeDefinition() const;
    QString actualRangeDefintion() const;
    QString attributename() const;
    QString attributeDomain() const;
    QString attributeValueType() const;
    Q_INVOKABLE QStringList attributeValues(bool defaultRange) const;
    Q_INVOKABLE QString rangeDefinition(bool defaultRange) const;



signals:

public slots:

private:
    Ilwis::ColumnDefinition _coldefinition;
    QStringList _attributeValuesDefaultRange;
    QStringList _attributeValuesActualRange;

};

#endif // ATTRIBUTEMODEL_H
