#include <QImage>
#include <QQuickFramebufferObject>
#include <QSGTexture>
#include <QQuickWindow>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "raster.h"
#include "table.h"
#include "pixeliterator.h"
#include "drawers/attributevisualproperties.h"
#include "drawers/drawerfactory.h"
#include "rootdrawer.h"
#include "itemrange.h"
#include "colorrange.h"
#include "layersrenderer.h"
#include "factory.h"
#include "abstractfactory.h"
#include "drawers/rasterimagefactory.h"
#include "drawers/rasterimage.h"
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

    IRasterCoverage raster = coverage().as<RasterCoverage>();
    if ( !_rasterImage){
        setActiveVisualAttribute(raster->hasAttributes() ? COVERAGEKEYCOLUMN : PIXELVALUE);
       _visualAttribute = visualProperty(activeAttribute());
       _rasterImage.reset(RasterImageFactory::create(raster->datadef().domain()->ilwisType(), rootDrawer(), raster,_visualAttribute,IOOptions()));
       if (!_rasterImage){
           ERROR2(ERR_NO_INITIALIZED_2,"RasterImage",raster->name());
           return false;
       }

       _maxTextureSize = 256; // = min(512, getMaxTextureSize());
       if ( raster->georeference().isValid() && raster->georeference()->isValid()) {
           _imageWidth = raster->georeference()->size().xsize();
           _imageHeight = raster->georeference()->size().ysize();
       } else if ( raster.isValid()) {
           _imageWidth = raster->size().xsize();
           _imageHeight = raster->size().ysize();
       }

       double log2width = log((double)_imageWidth)/log(2.0);
       log2width = max(6, ceil(log2width)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
       _width = pow(2, log2width);
       double log2height = log((double)_imageHeight)/log(2.0);
       log2height = max(6, ceil(log2height)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
       _height = pow(2, log2height);
    }
    if ( hasType(prepType, ptSHADERS) && !isPrepared(ptSHADERS)){
        _texturemat = _shaders.uniformLocation("texmat");
        _texcoordid = _shaders.attributeLocation("texCoord");
        _textureid = _shaders.uniformLocation( "tex" );
        _prepared |= DrawerInterface::ptSHADERS;
    }

    if ( hasType(prepType, DrawerInterface::ptGEOMETRY) && !isPrepared(DrawerInterface::ptGEOMETRY)){
        _prepared |= ( DrawerInterface::ptGEOMETRY);
    }
   if ( hasType(prepType, DrawerInterface::ptRENDER) && !isPrepared(DrawerInterface::ptRENDER)){
        setActiveVisualAttribute(raster->hasAttributes() ? activeAttribute() : PIXELVALUE);
       _visualAttribute = visualProperty(activeAttribute());
       _rasterImage->visualAttribute(_visualAttribute);
       _prepared |= ( DrawerInterface::ptRENDER);
   }
   _rasterImage->prepare((int)prepType);
   return true;
}

void RasterLayerDrawer::setActiveVisualAttribute(const QString &attr)
{
    IRasterCoverage raster = coverage().as<RasterCoverage>();
    if ( attr == PIXELVALUE){
        LayerDrawer::setActiveVisualAttribute(attr);
    }
    else if ( raster.isValid() ) {
        if(raster->hasAttributes()) {
            if ( raster->attributeTable()->columnIndex(attr) != iUNDEF){

                IRepresentation newrpr = Representation::defaultRepresentation(raster->attributeTable()->columndefinition(attr).datadef().domain());
                if ( newrpr.isValid()){
                    LayerDrawer::setActiveVisualAttribute(attr);
                }
            }
        }
    }
}

void RasterLayerDrawer::coverage(const ICoverage &cov)
{
    LayerDrawer::coverage(cov);

    IRasterCoverage raster = cov.as<RasterCoverage>();
    if (!raster.isValid())
        return;

    IlwisTypes attrType = raster->datadef().domain()->valueType();
    VisualAttribute attr(raster->datadef().domain());
    if ( hasType(attrType, itNUMBER)){
        QSharedPointer<Ilwis::NumericRange> numrange = raster->datadef().range<NumericRange>();
        attr.actualRange(NumericRange(numrange->min(), numrange->max(), numrange->resolution()));
        visualProperty(PIXELVALUE, attr);
    } else if ( hasType(attrType, itCONTINUOUSCOLOR)){
        visualProperty(PIXELVALUE, attr);
    }else if ( hasType(attrType, itPALETTECOLOR)){
        raster->datadef().range<ColorPalette>();
    }else if ( hasType(attrType, itDOMAINITEM)){
        //initialize all possible attributes that can be used in visualization
        for(int i = 0; i < raster->attributeTable()->columnCount(); ++i){
            const ColumnDefinition& coldef = raster->attributeTable()->columndefinitionRef(i);
            IlwisTypes attrType = coldef.datadef().domain()->ilwisType();
            if ( hasType(attrType, itNUMERICDOMAIN | itITEMDOMAIN | itTEXTDOMAIN)){
                VisualAttribute props = createVisualProperty(coldef, i);
                visualProperty(coldef.name(), props);
            }
        }

    }
    //set default
    setActiveVisualAttribute(raster->hasAttributes() ? COVERAGEKEYCOLUMN : PIXELVALUE);
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
    LayerDrawer::setAttribute(attrName, value);
    if ( attrName == "undefinedvalue"){
        coverage()->setPseudoUndef(value.toDouble());
    }
}

QVariant RasterLayerDrawer::attribute(const QString &attrName) const
{
    QVariant var = LayerDrawer::attribute(attrName);
    if ( var.isValid())
        return var;
    if ( attrName == "undefinedvalue"){
        return coverage()->pseudoUndef();
    }

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
    _shaders.setUniformValue(_vboAlpha, alpha());
    _shaders.setUniformValue(_textureid, 0); // all "bind" calls bind to GL_TEXTURE0
    DisplayImagePortion(0, 0, _width, _height);
    _shaders.release();

    return true;
}

#ifndef sqr
#define sqr(x) ((x) * (x))
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

void RasterLayerDrawer::DisplayImagePortion(unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY)
{
    // if patch describes the "added" portion of the map, do not display
    if (imageOffsetX > _imageWidth || imageOffsetY > _imageHeight)
        return;

    // if patch is outside viewport, do not display
    const IRasterCoverage & raster = coverage().as<RasterCoverage>();
    const IGeoReference & gr = raster->georeference();

    Coordinate b1 = gr->pixel2Coord(Pixel(imageOffsetX, imageOffsetY));
    Coordinate b2 = gr->pixel2Coord(Pixel(imageOffsetX + imageSizeX, imageOffsetY));
    Coordinate b3 = gr->pixel2Coord(Pixel(imageOffsetX + imageSizeX, imageOffsetY + imageSizeY));
    Coordinate b4 = gr->pixel2Coord(Pixel(imageOffsetX, imageOffsetY + imageSizeY));
    Coordinate c1 = rootDrawer()->coordinateSystem()->coord2coord(raster->coordinateSystem(), b1);
    Coordinate c2 = rootDrawer()->coordinateSystem()->coord2coord(raster->coordinateSystem(), b2);
    Coordinate c3 = rootDrawer()->coordinateSystem()->coord2coord(raster->coordinateSystem(), b3);
    Coordinate c4 = rootDrawer()->coordinateSystem()->coord2coord(raster->coordinateSystem(), b4);
    if (!(c1.isValid() && c2.isValid() && c3.isValid() && c4.isValid()))
        return;

    const QMatrix4x4 mvp = rootDrawer()->mvpMatrix();
    Size<> viewport = rootDrawer()->pixelAreaSize();
    QVector4D c1a (c1.x, c1.y, 0.0, 1.0);
    QVector4D c2a (c2.x, c2.y, 0.0, 1.0);
    QVector4D c3a (c3.x, c3.y, 0.0, 1.0);
    QVector4D c4a (c4.x, c4.y, 0.0, 1.0);
    QVector3D viewp (viewport.xsize(), viewport.ysize(), 1.0);
    QVector4D win1 = mvp * c1a;
    QVector4D win2 = mvp * c2a;
    QVector4D win3 = mvp * c3a;
    QVector4D win4 = mvp * c4a;
    win1 /= win1.w();
    win2 /= win2.w();
    win3 /= win3.w();
    win4 /= win4.w();
    win1 *= 0.5;
    win2 *= 0.5;
    win3 *= 0.5;
    win4 *= 0.5;
    win1 += QVector4D(0.5, 0.5, 0.5, 0.5);
    win2 += QVector4D(0.5, 0.5, 0.5, 0.5);
    win3 += QVector4D(0.5, 0.5, 0.5, 0.5);
    win4 += QVector4D(0.5, 0.5, 0.5, 0.5);
    win1 *= viewp;
    win2 *= viewp;
    win3 *= viewp;
    win4 *= viewp;

    geos::geom::GeometryFactory factory;
    const std::vector<geos::geom::Geometry *> holes;
    geos::geom::CoordinateArraySequence * coordsTile = new geos::geom::CoordinateArraySequence();
    coordsTile->add(Coordinate(win1.x(), win1.y()));
    coordsTile->add(Coordinate(win2.x(), win2.y()));
    coordsTile->add(Coordinate(win3.x(), win3.y()));
    coordsTile->add(Coordinate(win4.x(), win4.y()));
    coordsTile->add(Coordinate(win1.x(), win1.y()));
    const geos::geom::LinearRing ringTile(coordsTile, &factory);
    geos::geom::Polygon * polyTile(factory.createPolygon(ringTile, holes));

    geos::geom::CoordinateArraySequence * coordsViewport = new geos::geom::CoordinateArraySequence();
    coordsViewport->add(Coordinate(0, 0));
    coordsViewport->add(Coordinate(viewport.xsize(), 0));
    coordsViewport->add(Coordinate(viewport.xsize(), viewport.ysize()));
    coordsViewport->add(Coordinate(0, viewport.ysize()));
    coordsViewport->add(Coordinate(0, 0));
    const geos::geom::LinearRing ringViewport(coordsViewport, &factory);
    geos::geom::Polygon * polyViewport(factory.createPolygon(ringViewport, holes));

    bool fContains = !polyViewport->disjoint(polyTile);
    delete polyTile;
    delete polyViewport;
    if (!fContains)
        return;

    double screenPixelsY1 = sqrt(sqr(win2.x()-win1.x())+sqr(win2.y()-win1.y()));
    double screenPixelsX1 = sqrt(sqr(win3.x()-win2.x())+sqr(win3.y()-win2.y()));
    double screenPixelsY2 = sqrt(sqr(win4.x()-win3.x())+sqr(win4.y()-win3.y()));
    double screenPixelsX2 = sqrt(sqr(win1.x()-win4.x())+sqr(win1.y()-win4.y()));
    double zoom = min(imageSizeX/screenPixelsX1, min(imageSizeX/screenPixelsX2, min(imageSizeY/screenPixelsY1, imageSizeY/screenPixelsY2)));
    // the minimum zoomout-factor, indicating that it is necessary to plot the patch more accurately

    double log2zoom = log(zoom)/log(2.0);
    log2zoom = floor(log2zoom);
    const unsigned int zoomFactor = min(64, max(1, pow(2, log2zoom)));
    if (0 == zoomFactor)
        return;

    // split the visible portion of the image into a number of patches, depending on the accuracy needed

    bool xSplit = false;
    bool ySplit = false;

    if ((imageSizeX > 1) && (imageSizeX / zoomFactor > _maxTextureSize)) // imageSizeX / zoomFactor is the required pixels of the patch in the x-direction
        xSplit = true;
    if ((imageSizeY > 1) && (imageSizeY / zoomFactor > _maxTextureSize)) // imageSizeY / zoomFactor is the required pixels of the patch in the y-direction
        ySplit = true;
    if (xSplit && ySplit)
    {
        int sizeX2 = imageSizeX / 2;
        int sizeY2 = imageSizeY / 2;
        // Q1
        DisplayImagePortion(imageOffsetX, imageOffsetY, sizeX2, sizeY2);
        // Q2
        DisplayImagePortion(imageOffsetX + sizeX2, imageOffsetY, sizeX2, sizeY2);
        // Q3
        DisplayImagePortion(imageOffsetX + sizeX2, imageOffsetY + sizeY2, sizeX2, sizeY2);
        // Q4
        DisplayImagePortion(imageOffsetX, imageOffsetY + sizeY2, sizeX2, sizeY2);
    }
    else if (xSplit)
    {
        int sizeX2 = imageSizeX / 2;
        // Q1
        DisplayImagePortion(imageOffsetX, imageOffsetY, sizeX2, imageSizeY);
        // Q2
        DisplayImagePortion(imageOffsetX + sizeX2, imageOffsetY, sizeX2, imageSizeY);
    }
    else if (ySplit)
    {
        int sizeY2 = imageSizeY / 2;
        // Q1
        DisplayImagePortion(imageOffsetX, imageOffsetY, imageSizeX, sizeY2);
        // Q2
        DisplayImagePortion(imageOffsetX, imageOffsetY + sizeY2, imageSizeX, sizeY2);
    }
    else
    {
        //if (getRootDrawer()->is3D() && demTriangulator)
        //    DisplayTexture3D(c1, c2, c3, c4, imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor);
        //else
            DisplayTexture(c1, c2, c3, c4, imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor);
    }
}

void RasterLayerDrawer::DisplayTexture(Coordinate & c1, Coordinate & c2, Coordinate & c3, Coordinate & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor)
{
    if ( !_rasterImage)
        return;

    bool ok = _rasterImage->GetTexture(imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, _width, _height, zoomFactor, _shaders, _texturemat);
    if (ok)
    {
        const IRasterCoverage & raster = coverage().as<RasterCoverage>();
        const IGeoReference & gr = raster->georeference();
        QVector<QVector3D> vertices;
        QVector<QVector2D> texcoords;
        vertices.resize(4);
        texcoords.resize(4);

        bool fLinear = (rootDrawer()->coordinateSystem() == raster->coordinateSystem());
        if (fLinear) {
            double s1 = imageOffsetX / (double)_width;
            double t1 = imageOffsetY / (double)_height;
            double s2 = min(imageOffsetX + imageSizeX, _imageWidth) / (double)_width;
            double t2 = min(imageOffsetY + imageSizeY, _imageHeight) / (double)_height;

            Coordinate b1 = gr->pixel2Coord(Pixel(imageOffsetX, imageOffsetY));
            Coordinate b2 = gr->pixel2Coord(Pixel(min(imageOffsetX + imageSizeX, _imageWidth), imageOffsetY));
            Coordinate b3 = gr->pixel2Coord(Pixel(min(imageOffsetX + imageSizeX, _imageWidth), min(imageOffsetY + imageSizeY, _imageHeight)));
            Coordinate b4 = gr->pixel2Coord(Pixel(imageOffsetX, min(imageOffsetY + imageSizeY, _imageHeight)));

            c1 = rootDrawer()->coordinateSystem()->coord2coord(raster->coordinateSystem(), b1);
            c2 = rootDrawer()->coordinateSystem()->coord2coord(raster->coordinateSystem(), b2);
            c3 = rootDrawer()->coordinateSystem()->coord2coord(raster->coordinateSystem(), b3);
            c4 = rootDrawer()->coordinateSystem()->coord2coord(raster->coordinateSystem(), b4);

            vertices[0] = QVector3D(c1.x, c1.y, 0.0);
            vertices[1] = QVector3D(c2.x, c2.y, 0.0);
            vertices[2] = QVector3D(c3.x, c3.y, 0.0);
            vertices[3] = QVector3D(c4.x, c4.y, 0.0);
            texcoords[0] = QVector2D(s1, t1);
            texcoords[1] = QVector2D(s2, t1);
            texcoords[2] = QVector2D(s2, t2);
            texcoords[3] = QVector2D(s1, t2);

            _shaders.setAttributeArray( _vboPosition, vertices.constData() );
            _shaders.setAttributeArray( _texcoordid, texcoords.constData() );

            _shaders.enableAttributeArray( _vboPosition );
            _shaders.enableAttributeArray( _texcoordid );

            glDrawArrays( GL_QUADS, 0, 4 );

            _shaders.disableAttributeArray( _vboPosition );
            _shaders.disableAttributeArray( _texcoordid );
        } else {
            const unsigned int iSize = 10; // this makes 100 quads, thus 200 triangles per texture
            // avoid plotting the "added" portion of the map that was there to make the texture size a power of 2
            double colStep = min(imageSizeX, _imageWidth - imageOffsetX) / (double)iSize;
            double rowStep = min(imageSizeY, _imageHeight - imageOffsetY) / (double)iSize;

            double s1 = imageOffsetX / (double)_width;
            for (int x = 0; x < iSize; ++x) {
                double s2 = s1 + colStep / (double)_width;
                double t1 = imageOffsetY / (double)_height;

                Coordinate b1 = gr->pixel2Coord(Pixel(imageOffsetX + colStep * x, imageOffsetY));
                Coordinate b2 = gr->pixel2Coord(Pixel(imageOffsetX + colStep * (x + 1), imageOffsetY));

                c1 = rootDrawer()->coordinateSystem()->coord2coord(raster->coordinateSystem(), b1);
                c2 = rootDrawer()->coordinateSystem()->coord2coord(raster->coordinateSystem(), b2);
                for (int y = 1; y <= iSize ; ++y) {
                    double t2 = t1 + rowStep / (double)_height;

                    Coordinate b3 = gr->pixel2Coord(Pixel(imageOffsetX + colStep * (x + 1), imageOffsetY + rowStep * y));
                    Coordinate b4 = gr->pixel2Coord(Pixel(imageOffsetX + colStep * x, imageOffsetY + rowStep * y));

                    c3 = rootDrawer()->coordinateSystem()->coord2coord(raster->coordinateSystem(), b3);
                    c4 = rootDrawer()->coordinateSystem()->coord2coord(raster->coordinateSystem(), b4);

                    vertices[0] = QVector3D(c1.x, c1.y, 0.0);
                    vertices[1] = QVector3D(c2.x, c2.y, 0.0);
                    vertices[2] = QVector3D(c3.x, c3.y, 0.0);
                    vertices[3] = QVector3D(c4.x, c4.y, 0.0);
                    texcoords[0] = QVector2D(s1, t1);
                    texcoords[1] = QVector2D(s2, t1);
                    texcoords[2] = QVector2D(s2, t2);
                    texcoords[3] = QVector2D(s1, t2);

                    _shaders.setAttributeArray( _vboPosition, vertices.constData() );
                    _shaders.setAttributeArray( _texcoordid, texcoords.constData() );

                    _shaders.enableAttributeArray( _vboPosition );
                    _shaders.enableAttributeArray( _texcoordid );

                    glDrawArrays( GL_QUADS, 0, 4 );

                    _shaders.disableAttributeArray( _vboPosition );
                    _shaders.disableAttributeArray( _texcoordid );

                    t1 = t2;
                    c1 = c4;
                    c2 = c3;
                }
                s1 = s2;
            }
        }
    }
}
