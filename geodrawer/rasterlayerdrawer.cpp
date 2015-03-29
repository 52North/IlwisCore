#include <QImage>
#include <QQuickFramebufferObject>
#include <QSGTexture>
#include <QQuickWindow>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include "raster.h"
#include "drawers/attributevisualproperties.h"
#include "drawers/drawerfactory.h"
#include "rootdrawer.h"
#include "layersrenderer.h"
#include "rasterlayerdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_DRAWER(RasterLayerDrawer)

RasterLayerDrawer::~RasterLayerDrawer()
{

}


Ilwis::Geodrawer::RasterLayerDrawer::RasterLayerDrawer(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) :
    LayerDrawer("RasterLayerDrawer", parentDrawer, rootdrawer, options)
{
    _fragmentShader = "rasterfragmentshader_nvdia.glsl";
    _vertexShader = "rastervertexshader_nvdia.glsl";
}

bool RasterLayerDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options)
{
    if(!LayerDrawer::prepare(prepType, options))
        return false;

    if ( hasType(prepType, ptSHADERS) && !isPrepared(ptSHADERS)){
        _texcoordid = _shaders.attributeLocation("texCoord");
        _textureid = _shaders.uniformLocation( "tex" );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        _prepared |= DrawerInterface::ptSHADERS;
    }

    if ( hasType(prepType, DrawerInterface::ptGEOMETRY) && !isPrepared(DrawerInterface::ptGEOMETRY)){
        IRasterCoverage raster = coverage().as<RasterCoverage>();
        Size<> rastersize = raster->size();
        GLint texSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
        texSize = texSize == 0 ? 256 : texSize;
        double needed = std::log10(std::max(rastersize.xsize(), rastersize.ysize())) / std::log10(2);
        int maxpow2 = needed - (int)needed == 0 ? needed : needed + 1;
        int maxsizeneeded = std::pow(2,maxpow2);

        QImage img("h:/temp/af_dem_cor.bmp")  ;
        _texture.reset( new QOpenGLTexture(img));
        Envelope env = rootDrawer()->zoomEnvelope();
        _vertices.resize(4);
        _vertices[0] = QVector3D(env.min_corner().x, env.min_corner().y, 0);
        _vertices[1] = QVector3D(env.max_corner().x, env.min_corner().y, 0);
        _vertices[2] = QVector3D(env.max_corner().x, env.max_corner().y, 0);
        _vertices[3] = QVector3D(env.min_corner().x, env.max_corner().y, 0);
       // _vertices[4] = QVector3D(env.max_corner().x, env.max_corner().y, 0);
       // _vertices[5] = QVector3D(env.min_corner().x, env.max_corner().y, 0);

        _texcoords.resize(4);
        _texcoords = {{0,0},
                       {1,0},
                       {1,1},
                      {0,1}
                     };




    }
    return true;
}

void RasterLayerDrawer::setActiveVisualAttribute(const QString &attr)
{

}

void RasterLayerDrawer::coverage(const ICoverage &cov)
{
    LayerDrawer::coverage(cov);
    setActiveVisualAttribute(sUNDEF);

    IRasterCoverage raster = cov.as<RasterCoverage>();
    if (!raster.isValid())
        return;
    // fot the moment test only single band value maps with no attribute table; will extend when this works
    IlwisTypes attrType = raster->datadef().domain()->valueType();
    if ( !hasType(attrType, itNUMBER))
        return;

    visualAttribute(VisualAttribute::LAYERATTRIBUTE, VisualAttribute(raster->datadef().domain()));
}


ICoverage RasterLayerDrawer::coverage() const
{
    return SpatialDataDrawer::coverage();
}

DrawerInterface::DrawerType RasterLayerDrawer::drawerType() const
{
    return DrawerInterface::dtMAIN;
}

DrawerInterface *RasterLayerDrawer::create(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options)
{
    return new RasterLayerDrawer(parentDrawer, rootdrawer, options)    ;
}

void RasterLayerDrawer::setAttribute(const QString &attrName, const QVariant &value)
{

}

QVariant RasterLayerDrawer::attribute(const QString &attrName) const
{
    QVariant var = LayerDrawer::attribute(attrName);
    if ( var.isValid())
        return var;

    return QVariant();
}

bool RasterLayerDrawer::draw(const IOOptions &options)
{
    if ( !isActive())
        return false;

    if (!isPrepared()){
        return false;
    }

    if(!_shaders.bind())
        return false;
    QMatrix4x4 mvp = rootDrawer()->mvpMatrix();

   _shaders.setUniformValue(_modelview, mvp);


   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

   _texture->bind();

   _shaders.setAttributeArray( _vboPosition, _vertices.constData(), 3 );
   _shaders.setAttributeArray( _texcoordid, _texcoords.constData(), 2 );
   _shaders.setUniformValue( _textureid, 0 );

   _shaders.enableAttributeArray( _vboPosition );
   _shaders.enableAttributeArray( _texcoordid );

   glDrawArrays( GL_QUADS, 0, 4 );

   _shaders.disableAttributeArray( _vboPosition );
   _shaders.disableAttributeArray( _texcoordid );

    _shaders.release();

    return true;
}
