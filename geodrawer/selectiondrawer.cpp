#include "geometries.h"
#include "iooptions.h"
#include "drawerfactory.h"
#include "rootdrawer.h"
#include "selectiondrawer.h"

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
    if ( options.contains("color")){
        clr = options["color"].value<QColor>();
    }else {
        clr = QColor(0,0,255);
        clr.setAlphaF(1);
    }
    _colors = { clr, clr, clr, clr, clr};
    if ( x != rUNDEF && y !=  rUNDEF){
        VertexPosition pos(x,y);
        _vertices = { pos,pos,pos,pos, pos};
    }
    _indices.push_back(VertexIndex(0, 5, itLINE, iUNDEF));

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



    if ( _indices[0]._geomtype == itLINE){
        glDrawArrays(GL_LINE_STRIP,_indices[0]._start,_indices[0]._count);
    }
//    if ( _indices[0]._geomtype == itPOLYGON ){
//        glDrawArrays(GL_TRIANGLE_FAN,_indices[0]._start,_indices[0]._count);
//    }

    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    openglContext->functions()->glDisableVertexAttribArray(colorLocation);
    openglContext->functions()->glDisableVertexAttribArray(vertexLocation);

    _shaders.release();


     return true;
}

bool SelectionDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options, QOpenGLContext *openglContext)
{

    if ( hasType(prepType, DrawerInterface::ptMVP) && !isPrepared(DrawerInterface::ptMVP)){
        Size<> sz = rootDrawer()->pixelAreaSize();
        _projection.setToIdentity();
        _projection.ortho(QRect(0,0,sz.xsize(), sz.ysize()));
        _mvp = _model * _view * _projection;
        _prepared |= DrawerInterface::ptMVP;
    }

    if (!SimpleDrawer::prepare(prepType, options, openglContext)){
        return false;
    }

    if(!initGeometry(openglContext, _vertices, _colors))
        return false;

    return true;
}

void SelectionDrawer::setAttribute(const QString &attrName, const QVariant &attrib)
{
    if ( attrName == "currentx"){
        float value = attrib.toFloat();
        _vertices[1]._x = value;
        _vertices[2]._x = value;
    }
    if ( attrName == "currenty"){
        float value = attrib.toFloat();
        _vertices[2]._y = value;
        _vertices[3]._y = value;
    }
//    QVector3D v1(_vertices[0]._x,_vertices[0]._y,0);
//    QVector3D v2(_vertices[1]._x,_vertices[1]._y,0);
//    QVector3D v3(_vertices[2]._x,_vertices[2]._y,0);
//    QVector3D v4(_vertices[3]._x,_vertices[3]._y,0);

//    QVector3D v1a = _mvp * v1;
//    QVector3D v2a = _mvp * v2;
//    QVector3D v3a = _mvp * v3;
//    QVector3D v4a = _mvp * v4;
//    qDebug() << v1a.x() << v1a.y() << v2a.x() << v2a.y() << v3a.x() << v3a.y() << v4a.x() << v4a.y();
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
    return DrawerInterface::dtPRE;
}

quint32 SelectionDrawer::defaultOrder() const
{
    return 5;
}

const QMatrix4x4 &SelectionDrawer::mvpMatrix() const
{
    return _mvp;
}
