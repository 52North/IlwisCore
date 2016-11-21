#ifndef DomainItemModel_H
#define DomainItemModel_H

#include <QObject>

#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "identifierrange.h"
#include "interval.h"
#include "intervalrange.h"
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT DomainItemModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString code READ code CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString minimum READ min CONSTANT)
    Q_PROPERTY(QString maximum READ max CONSTANT)

public:
    DomainItemModel();
    explicit DomainItemModel(Ilwis::SPItemRange _range, const QString &name, QObject *parent = 0);

    QString name() const;
    QString code() const;
    QString description() const;
    QString min() const;
    QString max() const;

signals:

public slots:

private:
    Ilwis::SPItemRange _range;
    QString _itemname;

};

#endif // DomainItemModel_H
