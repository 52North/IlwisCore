#include "geometries.h"
#include "iooptions.h"
#include "drawers/drawerfactory.h"
#include "rootdrawer.h"
#include "mathhelper.h"
#include "griddrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

SubGridDrawer::SubGridDrawer(const QString& gridname, Ilwis::Geodrawer::DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) :
    SimpleDrawer(gridname, parentDrawer,rootdrawer, options),
    _lineColor(QColor("black"))
{
    _vertexShader = "featurevertexshader_nvdia.glsl";
    _fragmentShader = "featurefragmentshader_nvdia.glsl";
    valid(true); //TODO must move to the prepare but the order of things is not yet ok
}

bool SubGridDrawer::draw(const IOOptions &options)
{
    if ( !isActive())
        return false;

    if (!isPrepared()){
        return false;
    }

    if (!_shaders.bind())
        return false;

    float oldwidth;
    glGetFloatv(GL_LINE_WIDTH,&oldwidth);
     _mvp = rootDrawer()->mvpMatrix();
     _shaders.setUniformValue(_modelview,_mvp);
     _shaders.setUniformValue(_vboAlpha, alpha());

    _shaders.enableAttributeArray(_vboPosition);
    _shaders.enableAttributeArray(_vboNormal);
    _shaders.enableAttributeArray(_vboColor);

    _shaders.setAttributeArray(_vboPosition,_vertices.constData());
    _shaders.setAttributeArray(_vboNormal, _normals.constData());
    _shaders.setAttributeArray(_vboColor, GL_FLOAT, (void *)_colors.data(),4);
    _shaders.setUniformValue(_scaleFactor, 1.0f);
    glLineWidth(_linewidth);

//    GLfloat sizes[2];   // Store supported line width range
//    GLfloat step;       // Store supported line width increments

//    // Get supported line width range and step size
//    glGetFloatv(GL_LINE_WIDTH_RANGE,sizes);
//    glGetFloatv(GL_LINE_WIDTH_GRANULARITY,&step);

    for(const VertexIndex& index : _indices){
        glDrawArrays(GL_LINE_STRIP,index._start,index._count);
    }
    glLineWidth(oldwidth);
    _shaders.disableAttributeArray(_vboPosition);
    _shaders.disableAttributeArray(_vboNormal);
    _shaders.disableAttributeArray(_vboColor);

    return true;
}

bool SubGridDrawer::prepare(PreparationType prepType, const IOOptions &options)
{
    if (!SimpleDrawer::prepare(prepType, options)){
        return false;
    }
    if ( hasType(prepType, ptSHADERS) && !isPrepared(ptSHADERS)){
        _vboColor = _shaders.attributeLocation("vertexColor");
        _scaleCenter = _shaders.uniformLocation("scalecenter");
        _scaleFactor = _shaders.uniformLocation("scalefactor");

        _prepared |= DrawerInterface::ptSHADERS;
    }

    if ( hasType(prepType, DrawerInterface::ptRENDER) && !isPrepared(DrawerInterface::ptRENDER)){
        _colors = std::vector<VertexColor>();
        _colors.resize(_vertices.size());
        _lineColor.setAlphaF(_opacity);
        for(int i = 0; i < _vertices.size(); ++i){
            _colors[i] ={_lineColor};
        }
        _prepared |= DrawerInterface::ptRENDER;
    }
    return true;
}

void SubGridDrawer::setAttribute(const QString &attrName, const QVariant &attrib)
{
   SimpleDrawer::setAttribute(attrName, attrib);

   if ( attrName == "gridlineopacity"){
       _opacity = attrib.toDouble();
   }else if ( attrName == "gridlinecolor")
       _lineColor = attrib.value<QColor>();
   else if ( attrName == "gridcelldistance"){
       _cellDistance = attrib.toDouble();
   }
}

QVariant SubGridDrawer::attribute(const QString &attrName) const
{
    QVariant var = SimpleDrawer::attribute(attrName);
    if ( var.isValid())
        return var;
    if ( attrName == "gridlinecolor")
        return qVariantFromValue(_lineColor);
    if ( attrName == "gridlineopacity")
        return qVariantFromValue(_opacity);
    if ( attrName == "gridcelldistance")
        return qVariantFromValue(_cellDistance);

    return QVariant();
}

DrawerInterface::DrawerType SubGridDrawer::drawerType() const
{
   return DrawerInterface::dtMAIN;
}

//-------------------------------------------------------------------------------------------------------
GridDrawer::GridDrawer(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) :
 ComplexDrawer("GridDrawer", parentDrawer,rootdrawer, options)
{
    valid(true);
}

bool GridDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options)
{
    if (!hasDrawer("PrimaryGridDrawer")){
        SubGridDrawer *primarygrid = new PrimaryGridDrawer(this,rootDrawer(),options);
        primarygrid->prepare(DrawerInterface::ptALL, options);
        addDrawer(primarygrid,DrawerInterface::dtMAIN,iUNDEF,"PrimaryGridDrawer");
        SubGridDrawer *secondarygrid = new SecondaryGridDrawer(this,rootDrawer(),options);
        secondarygrid->prepare(DrawerInterface::ptALL, options);
        addDrawer(secondarygrid,DrawerInterface::dtMAIN,iUNDEF,"SecondaryGridDrawer");
    }

    _prepared |= DrawerInterface::ptSHADERS; // there are no shaders in this drawer, childeren do the drawing
    return ComplexDrawer::prepare(prepType, options);
}

DrawerInterface::DrawerType GridDrawer::drawerType() const
{
    return DrawerInterface::dtPOST;
}

quint32 GridDrawer::defaultOrder() const
{
    return 20;
}

Envelope GridDrawer::envelope() const
{
    return rootDrawer()->coverageEnvelope();
}

//-----------------------------------------------------------------------
PrimaryGridDrawer::PrimaryGridDrawer(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) :
    SubGridDrawer("PrimaryGridDrawer",parentDrawer, rootdrawer, options)
{
    _lineColor = {"black"};
}

bool PrimaryGridDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options)
{

    if ( hasType(prepType, DrawerInterface::ptGEOMETRY) && !isPrepared(DrawerInterface::ptGEOMETRY)){
        if ( _cellDistance == rUNDEF){
            Coordinate cmin = rootDrawer()->coverageEnvelope().min_corner();
            Coordinate cmax = rootDrawer()->coverageEnvelope().max_corner();
            _cellDistance = MathHelper::round((cmax.x - cmin.x) / 7.0);
        }
        _indices = std::vector<VertexIndex>();
        _vertices = QVector<QVector3D>();
        _normals = QVector<QVector3D>();
        _linewidth = 0.75;
        if ( _opacity == rUNDEF)
            _opacity = 0.8;

        Coordinate cmin = rootDrawer()->coverageEnvelope().min_corner();
        Coordinate cmax = rootDrawer()->coverageEnvelope().max_corner();

        _indices.push_back(VertexIndex(_vertices.size(),5, itLINE));
        _vertices.push_back(QVector3D(cmin.x,cmin.y,0));
        _vertices.push_back(QVector3D(cmin.x,cmax.y,0));
        _vertices.push_back(QVector3D(cmax.x,cmax.y,0));
        _vertices.push_back(QVector3D(cmax.x,cmin.y,0));
        _vertices.push_back(QVector3D(cmin.x,cmin.y,0));

        for (double x = ceil(cmin.x / _cellDistance) * _cellDistance; x < cmax.x ; x += _cellDistance)
        {
            _indices.push_back(VertexIndex(_vertices.size(),2, itLINE));
            _vertices.push_back(QVector3D(x,cmin.y,0));
            _vertices.push_back(QVector3D(x,cmax.y,0));
        }
        double y;
        for (y = ceil(cmin.y / _cellDistance) * _cellDistance; y < cmax.y ; y += _cellDistance)
        {
            _indices.push_back(VertexIndex(_vertices.size(),2, itLINE));
            _vertices.push_back(QVector3D(cmin.x,y,0));
            _vertices.push_back(QVector3D(cmax.x,y,0));
        }

        _prepared |= DrawerInterface::ptGEOMETRY;
        // as colors are directly linked to vertices any change in geometry must trigger a render prepare
        unprepare(DrawerInterface::ptRENDER);
        SubGridDrawer::prepare(DrawerInterface::ptRENDER, options);
    }
    if(!SubGridDrawer::prepare(prepType,options))
        return false;
    return true;
}

//-----------------------------------------------------------------------
SecondaryGridDrawer::SecondaryGridDrawer(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) :
    SubGridDrawer("SecondaryGridDrawer",parentDrawer, rootdrawer, options)
{
    _lineColor = {"DarkBlue"};
}

bool SecondaryGridDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options)
{

    if ( hasType(prepType, DrawerInterface::ptGEOMETRY) && !isPrepared(DrawerInterface::ptGEOMETRY)){
        const UPDrawer& primarydrawer = parent()->drawer("PrimaryGridDrawer");
        double dist = primarydrawer->attribute("gridcelldistance").toDouble();
        _cellDistance = dist / _cellCount;
        _indices = std::vector<VertexIndex>();
        _vertices = QVector<QVector3D>();
        _normals = QVector<QVector3D>();
        if ( _opacity == rUNDEF)
            _opacity = 0.5;
       _linewidth = 0.5;

        Coordinate cmin = rootDrawer()->coverageEnvelope().min_corner();
        Coordinate cmax = rootDrawer()->coverageEnvelope().max_corner();
        double xstart = ceil(cmin.x / dist) * dist - dist;
        for (double x = xstart; x < cmax.x ; x += dist)
        {
            for(int i = 1 ; i < _cellCount; ++i){
                double newx = x + i * _cellDistance;
                if ( newx > cmin.x && newx < cmax.x){
                    _indices.push_back(VertexIndex(_vertices.size(),2, itLINE));
                    _vertices.push_back(QVector3D(newx ,cmin.y,0));
                    _vertices.push_back(QVector3D(newx ,cmax.y,0));
                }
            }
        }
        double ystart = ceil(cmin.y / dist) * dist - dist;
        for (double y = ystart; y < cmax.y ; y += dist)
        {
            for(int i = 1; i < _cellCount; ++i){
                double newy = y + i * _cellDistance;
                if ( newy > cmin.y && newy < cmax.y){
                    _indices.push_back(VertexIndex(_vertices.size(),2, itLINE));
                    _vertices.push_back(QVector3D(cmin.x,newy,0));
                    _vertices.push_back(QVector3D(cmax.x,newy,0));
                }
            }
        }
        _prepared |= DrawerInterface::ptGEOMETRY;
        // as colors are directly linked to vertices any change in geometry must trigger a render prepare
        unprepare(DrawerInterface::ptRENDER);
        SubGridDrawer::prepare(DrawerInterface::ptRENDER, options);

    }
    if(!SubGridDrawer::prepare(prepType,options))
        return false;
    return true;
}

void SecondaryGridDrawer::setAttribute(const QString &attrName, const QVariant &attrib)
{
   SubGridDrawer::setAttribute(attrName, attrib);

   if ( attrName == "cellcount"){
       _cellCount = attrib.toDouble();
    }
}

QVariant SecondaryGridDrawer::attribute(const QString &attrName) const
{
    QVariant var = SubGridDrawer::attribute(attrName);
    if ( var.isValid())
        return var;
    if ( attrName == "cellcount")
        return qVariantFromValue(_cellCount);

    return QVariant();
}

bool SecondaryGridDrawer::draw(const IOOptions &options)
{
    const UPDrawer& primarydrawer = parent()->drawer("PrimaryGridDrawer");
    if ( !primarydrawer->isActive())
        return false;

    return SubGridDrawer::draw(options);
}
