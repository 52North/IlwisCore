#ifndef POSTGRESQLTABLECONNECTOR_H
#define POSTGRESQLTABLECONNECTOR_H

namespace Ilwis {

class ColumnDefinition;

namespace Postgresql {
class PostgresqlTableConnector : public PostgresqlConnector
{
public:
    PostgresqlTableConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());


    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data,const IOOptions& options= IOOptions());
    bool loadData(IlwisObject *data, const IOOptions& options= IOOptions());
    bool store(Ilwis::IlwisObject *data, const IOOptions &options);

private:
    QString trimAndRemoveLastCharacter(const QString &string);
    QString createInsertValueString(QVariant value, const ColumnDefinition &coldef) const;

};
}
}

#endif // POSTGRESQLTABLECONNECTOR_H
