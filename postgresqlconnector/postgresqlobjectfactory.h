#ifndef POSTGRESQLOBJECTFACTORY_H
#define POSTGRESQLOBJECTFACTORY_H

namespace Ilwis {
namespace Postgresql {

class PostgresqlObjectFactory : public IlwisObjectFactory
{
public:
    PostgresqlObjectFactory();

    bool canUse(const Resource &resource) const;
    IlwisObject *create(const Resource &resource,const IOOptions& options=IOOptions()) const;
};
}
}

#endif // POSTGRESQLOBJECTFACTORY_H
