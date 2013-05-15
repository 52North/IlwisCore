#ifndef RESOURCE_H
#define RESOURCE_H

#include "Kernel_global.h"
#include <QVariant>

class QSqlRecord;
class QSqlQuery;

#define INTERNAL_OBJECT (QUrl("ilwis://internal"))

namespace Ilwis {
class KERNELSHARED_EXPORT Resource : public Identity
{
public:
    Resource();
    Resource(const QUrl& url, quint64 tp, bool isNew=true);
    Resource(const QString &code, quint64 tp, bool isNew=true);
    Resource(quint64 tp, const QUrl& url=INTERNAL_OBJECT);
    Resource(const QSqlRecord& rec);
    virtual ~Resource() {}

    QVariant operator[] (const QString& property) const;
    bool hasProperty(const QString& property) const;
    void addProperty(const QString& key, const QVariant& value );
    QUrl url() const;
    void setUrl(const QUrl& url);
    QString toLocalFile(bool relative=false) const;
    QUrl container() const;
    void setContainer(const QUrl &url);
    quint64 size() const;
    QString dimensions() const;
    IlwisTypes ilwisType() const;
    IlwisTypes extendedType() const;
    void setIlwisType(IlwisTypes tp);
    void setExtendedType(IlwisTypes tp);
    void prepare();
    bool store(QSqlQuery &queryItem, QSqlQuery &queryProperties) const;
    bool isValid() const;
    bool operator()(const Ilwis::Resource& resource);

    static QString toLocalFile(const QUrl& url, bool relative=false) ;

protected:
    QHash<QString, QVariant> _properties;
    QUrl _resource;
    QUrl _container;
    quint64 _size;
    QString _dimensions;
    IlwisTypes _ilwtype;
    IlwisTypes _extendedType;

    void checkUrl(IlwisTypes tp);

};



KERNELSHARED_EXPORT uint qHash(const Ilwis::Resource& resource );
KERNELSHARED_EXPORT uint qHash2(const QUrl& url, IlwisTypes tp );
KERNELSHARED_EXPORT bool operator==(const Ilwis::Resource& resource1, const Ilwis::Resource& resource2 );
KERNELSHARED_EXPORT bool operator<(const Ilwis::Resource& resource1, const Ilwis::Resource& resource2 );

}

#endif // RESOURCE_H
