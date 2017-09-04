#ifndef ILWIS3OBJECTFACTORY_H
#define ILWIS3OBJECTFACTORY_H

namespace Ilwis {
class Ilwis3ObjectFactory : public IlwisObjectFactory
{
public:
    Ilwis3ObjectFactory();

    IlwisObject *create(const Ilwis::Resource &resource,const IOOptions& options=IOOptions()) const;
    bool canUse(const Resource &resource) const;
    bool prepare();
    std::vector<Resource> loadResource(const QUrl &url, IlwisTypes types=itILWISOBJECT) const;
private:
    void loadIlwis3Aliasses();
    void insertFile(const QString &filename, Ilwis::InternalDatabaseConnection &internalDb);
    bool fillEllipsoidRecord(const QStringList &parts, InternalDatabaseConnection &internalDb);
    bool fillDatumRecord(const QStringList &parts, InternalDatabaseConnection &internalDb);
    bool fillProjectionRecord(const QStringList &parts, InternalDatabaseConnection &internalDb);
    bool fillDomainRecord(const QStringList &parts, InternalDatabaseConnection &internalDb);
    bool doQuery(QString& query, InternalDatabaseConnection &internalDb);

    QHash<IlwisTypes,CheckUsage> _types;
};
}

#endif // ILWIS3OBJECTFACTORY_H
