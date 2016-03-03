#ifndef INTERNALDATABASEMODEL_H
#define INTERNALDATABASEMODEL_H

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT InternalDatabaseModel : public QObject
{
    Q_OBJECT
public:
    explicit InternalDatabaseModel(QObject *parent = 0);

    Q_INVOKABLE QVariantList query(const QString& query);

signals:

public slots:
};

#endif // INTERNALDATABASEMODEL_H
