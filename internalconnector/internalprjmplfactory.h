#ifndef ProjectionImplFactory_H
#define ProjectionImplFactory_H

namespace Ilwis {
namespace Internal {
class ProjectionImplFactory : public ProjectionFactory
{
public:
    ProjectionImplFactory();

    ProjectionImplementation *create(const Ilwis::Resource &resource) const;
    ProjectionImplementation *create(const QString&, const QString &) const;
    virtual bool canUse(const Ilwis::Resource &resource) const;
    virtual bool prepare();
private:
    QString _knownProjections;

};
}
}

#endif // ProjectionImplFactory
