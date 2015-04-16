#ifndef RASTERIMAGEFACTORY_H
#define RASTERIMAGEFACTORY_H

#include "ilwiscoreui_global.h"

namespace Ilwis {
class RasterCoverage;
typedef IlwisData<RasterCoverage> IRasterCoverage;

namespace Geodrawer{

class RasterImage;
class RootDrawer;

class VisualAttribute;

typedef std::function<RasterImage *(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute,const IOOptions &options)> CreateRasterImage;

class ILWISCOREUISHARED_EXPORT RasterImageFactory : public AbstractFactory
{
public:
    RasterImageFactory();
    ~RasterImageFactory();

    static RasterImage * registerRasterImageType(const QString& name, IlwisTypes type, CreateRasterImage createFunc);

    template<class T=RasterImage> static T *create(const QString &name, DrawerInterface *rootdrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute, const IOOptions &options)
    {
       auto iter = _creatorsByName.find(name.toLower());
       if ( iter == _creatorsByName.end()){
           return 0;
       }
       return static_cast<T *>((*iter).second(rootdrawer, raster,vattribute, options));
    }

    template<class T=RasterImage> static T *create(IlwisTypes tp, DrawerInterface *rootdrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute, const IOOptions &options)
    {
       auto iter = _creatorsByType.find(tp);
       if ( iter == _creatorsByType.end()){
           return 0;
       }
       return static_cast<T *>((*iter).second(rootdrawer, raster,vattribute, options));
    }

private:
    static std::map<QString, CreateRasterImage>  _creatorsByName;
    static std::map<IlwisTypes, CreateRasterImage>  _creatorsByType;
};
}
}

#endif // RASTERIMAGEFACTORY_H
