#include <QOpenGLTexture>
#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "attributevisualproperties.h"
#include "drawerinterface.h"
#include "rasterimage.h"
#include "rasterimagefactory.h"

using namespace Ilwis;
using namespace Geodrawer;

std::map<QString, CreateRasterImage>  RasterImageFactory::_creatorsByName;
std::map<IlwisTypes, CreateRasterImage>  RasterImageFactory::_creatorsByType;

RasterImageFactory::RasterImageFactory() : AbstractFactory("rasterimagefactory","ilwis","Factory to create raster image objects that contain the main functionality of translating rasters into textures")
{

}

RasterImageFactory::~RasterImageFactory()
{

}

RasterImage *RasterImageFactory::registerRasterImageType(const QString &name, IlwisTypes type, CreateRasterImage createFunc)
{
    auto iter = _creatorsByName.find(name.toLower());
    if ( iter == _creatorsByName.end()){
        _creatorsByName[name.toLower()] = createFunc;
    }

    auto iter2 = _creatorsByType.find(type);
    if ( iter2 == _creatorsByType.end()){
        _creatorsByType[type] = createFunc;
    }
    return 0;
}

