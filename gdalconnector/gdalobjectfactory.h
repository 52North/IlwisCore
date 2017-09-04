#ifndef GDALOBJECTFACTORY_H
#define GDALOBJECTFACTORY_H

namespace Ilwis {
namespace Gdal{
class GdalObjectFactory : public IlwisObjectFactory
{
public:
    GdalObjectFactory();
    bool canUse(const Resource &resource) const;
    IlwisObject *create(const Resource &resource,const IOOptions& options=IOOptions()) const;
};
}
}

#endif // GDALOBJECTFACTORY_H
