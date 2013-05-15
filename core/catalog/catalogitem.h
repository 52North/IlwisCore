#ifndef CATALOGITEM_H
#define CATALOGITEM_H

#include "Kernel_global.h"
#include <QVariant>

class QSqlRecord;
class QSqlQuery;

namespace Ilwis {
class KERNELSHARED_EXPORT CatalogItem : public Identity
{
public:
    CatalogItem();
    CatalogItem(const QUrl& url);

    CatalogItem(const QSqlRecord& rec);

    QString operator[] (const QString& property) const;
    QVariant& operator[] (const QString& property);
    bool hasProperty(const QString& property) const;
    QUrl url() const;
    QUrl container() const;
    void setContainer(const QUrl &url);
    quint64 size() const;
    QString dimensions() const;
    IlwisTypes ilwisType() const;
    IlwisTypes extendedType() const;
    QString type() const;
    void setIlwisType(IlwisTypes tp);
    void setExtendedType(IlwisTypes tp);
    void prepare();
    bool store(QSqlQuery &queryItem, QSqlQuery &queryProperties) const;
    bool isValid() const;

protected:
    QHash<QString, QVariant> _properties;
    QUrl _resource;
    QUrl _container;
    quint64 _size;
    QString _dimensions;
    IlwisTypes _ilwtype;
    IlwisTypes _extendedType;
};

uint qHash(const Ilwis::CatalogItem& resource );
bool operator==(const Ilwis::CatalogItem& resource1, const Ilwis::CatalogItem& resource2 );
bool operator<(const Ilwis::CatalogItem& resource1, const Ilwis::CatalogItem& resource2 );
}

#endif // CATALOGITEM2_H
