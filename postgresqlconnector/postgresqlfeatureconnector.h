#ifndef POSTGRESQLFEATURECONNECTOR_H
#define POSTGRESQLFEATURECONNECTOR_H

namespace Ilwis {

class FeatureCoverage;

namespace Postgresql {
class PostgresqlFeatureConnector : public PostgresqlConnector
{
public:
    PostgresqlFeatureConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());
    ~PostgresqlFeatureConnector();


    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data,const IOOptions& options=IOOptions());
    bool loadData(IlwisObject *data, const IOOptions &options=IOOptions());
    bool store(IlwisObject* data, const IOOptions &options);

private:
    QString trimAndRemoveLastCharacter(const QString &string);

    IOOptions merge(const IOOptions &first, const IOOptions &second);

};
}
}

#endif // POSTGRESQLFEATURECONNECTOR_H
