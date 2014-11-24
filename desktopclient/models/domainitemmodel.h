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

class DomainItemModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString code READ code CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
public:
    DomainItemModel();
    explicit DomainItemModel(Ilwis::SPItemRange _range, const QString &name, QObject *parent = 0);

    QString name() const;
    QString code() const;
    QString description() const;

signals:

public slots:

private:
    Ilwis::SPItemRange _range;
    QString _itemname;

};

#endif // DomainItemModel_H
