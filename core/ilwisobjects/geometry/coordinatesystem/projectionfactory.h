#ifndef PROJECTIONFACTORY_H
#define PROJECTIONFACTORY_H

namespace Ilwis {
class ProjectionFactory : public AbstractFactory
{
public:
    ProjectionFactory(const QString& ty, const QString& sub, const QString& desc="") : AbstractFactory(ty, sub, desc) {}

    virtual ProjectionImplementation *create(const Resource &resource) const = 0;
    virtual ProjectionImplementation *create(const QString& projtype, const QString& name) const = 0;
};
}

#endif // PROJECTIONFACTORY_H
