#include "geometries.h"
#include "iooptions.h"
#include "drawerfactory.h"
#include "rootdrawer.h"
#include "selectiondrawer.h"
#include "boost/math/special_functions/sign.hpp"

using namespace Ilwis;
using namespace Geodrawer;

//REGISTER_DRAWER(SelectionDrawer)

SelectionDrawer::SelectionDrawer(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) : SimpleDrawer("SelectionDrawer", parentDrawer,rootdrawer, options)
{
    float x=flUNDEF,y = flUNDEF;
    bool ok;
    if ( options.contains("currentx")){
        x = options["currentx"].toFloat(&ok);
        if ( !ok)
            x = flUNDEF;
    }
    if ( options.contains("currenty")){
        y = options["currenty"].toFloat(&ok);
        if (!ok)
            y = flUNDEF;
    }
    QColor clr;
    _colors.resize(13);
    if ( options.contains("areacolor")){
        clr = options["areacolor"].value<QColor>();
    }else {
        clr = QColor(0,0,128);
        clr.setAlphaF(0.05);
    }
    _colors[9] = _colors[10] = _colors[11] = _colors[12] = clr;
    clr.setAlphaF(0.1);
    _colors[5] = _colors[6] = _colors[7] = _colors[8] = clr;

    if ( options.contains("bordercolor")){
        clr = options["bordercolor"].value<QColor>();
    }else {
        clr = QColor(0,0,255);
        clr.setAlphaF(1);
    }
    _colors[0] = _colors[1] = _colors[2] = _colors[3] = _colors[4] = clr;

    if ( x != rUNDEF && y !=  rUNDEF){
        VertexPosition pos(x,y);
        _vertices = { pos,pos,pos,pos,pos, pos,pos,pos,pos, pos,pos,pos,pos};
    }
    _indices.push_back(VertexIndex(0, 5, itLINE, iUNDEF));
    _indices.push_back(VertexIndex(5, 2, itLINE, iUNDEF));
    _indices.push_back(VertexIndex(7, 2, itLINE, iUNDEF));
    _indices.push_back(VertexIndex(9, 4, itPOLYGON, iUNDEF));
}

SelectionDrawer::~SelectionDrawer()
{

}

bool SelectionDrawer::draw(QOpenGLContext *openglContext, const IOOptions &)
{

    if ( !openglContext){
        return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR("Invalid OpenGL context passed"));
    }

    if ( !isActive())
        return false;

    if (!isPrepared()){
        return false;
    }

    if (!_shaders.bind())
        return false;

    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER,_vboPosition);

    int vertexLocation = _shaders.attributeLocation("position");
    openglContext->functions()->glVertexAttribPointer(vertexLocation,3,GL_FLOAT,FALSE,0,0);
    openglContext->functions()->glEnableVertexAttribArray(vertexLocation);

    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER,_vboColor);

    int colorLocation = _shaders.attributeLocation("vertexColor");
    openglContext->functions()->glVertexAttribPointer(colorLocation,4,GL_FLOAT,FALSE,0, 0);
    openglContext->functions()->glEnableVertexAttribArray(colorLocation);



    glDrawArrays(GL_LINE_STRIP,_indices[0]._start,_indices[0]._count);
    glDrawArrays(GL_LINE_STRIP,_indices[1]._start,_indices[1]._count);
    glDrawArrays(GL_LINE_STRIP,_indices[2]._start,_indices[2]._count);
    glDrawArrays(GL_TRIANGLE_FAN,_indices[3]._start,_indices[3]._count);


    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    openglContext->functions()->glDisableVertexAttribArray(colorLocation);
    openglContext->functions()->glDisableVertexAttribArray(vertexLocation);

    _shaders.release();


     return true;
}

bool SelectionDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options, QOpenGLContext *openglContext)
{
    if (!SimpleDrawer::prepare(prepType, options, openglContext)){
        return false;
    }

    if ( hasType(prepType, DrawerInterface::ptGEOMETRY) && !isPrepared(DrawerInterface::ptGEOMETRY)){
        if(!moveGeometry2GPU(openglContext, _vertices, _colors))
            return false;
        _prepared |= ptGEOMETRY;
    }

    if ( hasType(prepType, DrawerInterface::ptMVP) && !isPrepared(DrawerInterface::ptMVP) ){

        if (!_shaders.bind())
            return false;

        Size<> sz = rootDrawer()->pixelAreaSize();
        _projection.setToIdentity();
        _projection.ortho(QRect(0,0,sz.xsize(), sz.ysize()));
        _mvp = _model * _view * _projection;

        _shaders.setUniformValue("mvp",_mvp);
        _shaders.enableAttributeArray("mvp");

        _prepared |= DrawerInterface::ptMVP;
        _shaders.release();
    }

    return true;
}

void SelectionDrawer::setAttribute(const QString &attrName, const QVariant &attrib)
{
    if ( attrName == "preserveaspectration"){
        _preserveAspectRatio = attrib.toBool();
    }

    if ( attrName == "currentx"){
        float value = attrib.toFloat();
        _vertices[1]._x = value;
        _vertices[2]._x = value;
        _vertices[10]._x = value;
        _vertices[11]._x = value;


    }
    if ( attrName == "currenty"){
        float value = attrib.toFloat();
        _vertices[2]._y = value;
        _vertices[3]._y = value;
        _vertices[11]._y = value;
        _vertices[12]._y = value;

    }
    if ( _preserveAspectRatio){
        double aspectRatioView = rootDrawer()->aspectRatioView();
        double dx = _vertices[1]._x - _vertices[0]._x;
        double dy = _vertices[3]._y - _vertices[0]._y;
        if ( aspectRatioView <= 1){
            _vertices[11]._y = _vertices[12]._y = _vertices[2]._y = _vertices[3]._y = _vertices[0]._y + boost::math::sign(dy) * dx/ aspectRatioView;

        }else {
            _vertices[10]._x = _vertices[11]._x = _vertices[1]._x = _vertices[2]._x = _vertices[0]._x + boost::math::sign(dx) * dy * aspectRatioView;
        }
    }
    _vertices[7]._y = _vertices[8]._y = (_vertices[0]._y + _vertices[2]._y) /2;
    _vertices[6]._y = _vertices[2]._y;
    _vertices[5]._x = _vertices[6]._x = (_vertices[0]._x + _vertices[1]._x) /2;
    _vertices[8]._x = _vertices[1]._x;
    unprepare(ptGEOMETRY);
}

bool SelectionDrawer::drawerAttribute(const QString drawercode, const QString &attrName, const QVariant &attrib)
{
    if (drawercode != code())
        return false;
    setAttribute(attrName, attrib);

    return true;
}

DrawerInterface *SelectionDrawer::create(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options)
{
       return new SelectionDrawer(parentDrawer, rootdrawer, options)    ;
}

DrawerInterface::DrawerType SelectionDrawer::drawerType() const
{
    return DrawerInterface::dtPOST;
}

quint32 SelectionDrawer::defaultOrder() const
{
    return 100;
}

Envelope SelectionDrawer::envelope() const
{
    QVector3D v1( _vertices[0]._x, _vertices[0]._y, _vertices[0]._z);
    QVector3D v2( _vertices[2]._x, _vertices[2]._y, _vertices[2]._z);
    auto v1normalized = _mvp * v1;
    auto v2normalized = _mvp * v2;
    const auto& globalMvp = rootDrawer()->mvpMatrix();
    double a11 = globalMvp.row(0).x();
    double a14 = globalMvp.row(0).w();
    double a22 = globalMvp.row(1).y();
    double a24 = globalMvp.row(1).w();
    Coordinate crd1((v1normalized.x() - a14) / a11, (v1normalized.y() - a24)/ a22);
    Coordinate crd2((v2normalized.x() - a14) / a11, (v2normalized.y() - a24)/ a22);

    return Envelope(crd1, crd2);
}

void SelectionDrawer::cleanUp(QOpenGLContext *openglContext)
{
    SimpleDrawer::cleanUp(openglContext);
    rootDrawer()->applyEnvelopeZoom(envelope());
}


