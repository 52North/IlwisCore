#ifndef ATTRIBUTEMODEL_H
#define ATTRIBUTEMODEL_H

#include <QObject>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT AttributeModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString attributename READ attributename CONSTANT)
    Q_PROPERTY(QString defaultRangeDefinition READ defaultRangeDefinition CONSTANT)
    Q_PROPERTY(QString actualRangeDefintion READ actualRangeDefintion CONSTANT)
    Q_PROPERTY(QString attributeDomain READ attributeDomain CONSTANT)
    Q_PROPERTY(QString attributeValueType READ attributeValueType CONSTANT)
    Q_PROPERTY(QString attributeDomainType READ attributeDomainType CONSTANT)

public:
    AttributeModel();
    virtual ~AttributeModel();
    explicit AttributeModel(const Ilwis::ColumnDefinition& def, QObject *parent, const Ilwis::IIlwisObject& obj);

    QString defaultRangeDefinition() const;
    QString actualRangeDefintion(bool calc=false) const;
    virtual QString attributename() const;
    QString attributeDomain() const;
    QString attributeValueType() const;
    QString attributeDomainType() const;
    Q_INVOKABLE QStringList attributeValues(bool defaultRange) const;
    Q_INVOKABLE QString rangeDefinition(bool defaultRange, bool calc, const QString&) const;

   const Ilwis::ColumnDefinition& columnDef() const;

protected:
    void setObject(const Ilwis::IIlwisObject& obj);

    Ilwis::ColumnDefinition _coldefinition;
    QStringList _attributeValuesDefaultRange;
    QStringList _attributeValuesActualRange;
    Ilwis::IIlwisObject _owner;

signals:

public slots:



};

#endif // ATTRIBUTEMODEL_H
