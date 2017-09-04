#ifndef POSTGRESQLCONNECTOR_H
#define POSTGRESQLCONNECTOR_H

namespace Ilwis {
namespace Postgresql {

class PostgresqlConnector : public IlwisObjectConnector
{
public:
    PostgresqlConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    virtual ~PostgresqlConnector();

    QString provider() const;

private:

};
}
}

#endif // POSTGRESQLCONNECTOR_H
