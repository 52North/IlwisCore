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
        clr.setAlphaF(0.5);
    }
    _colors = { clr, clr, clr, clr, clr};
    if ( x != rUNDEF && y !=  rUNDEF){
        VertexPosition pos(x,y);
        _vertices = { pos,pos,pos,pos, pos};
    }
    _indices.push_back(VertexIndex(0, 5, itPOLYGON, iUNDEF));

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

    _shaders.bind();

    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER,_vboPosition);

    int vertexLocation = _shaders.attributeLocation("position");
    openglContext->functions()->glVertexAttribPointer(vertexLocation,3,GL_FLOAT,FALSE,0,0);
    openglContext->functions()->glEnableVertexAttribArray(vertexLocation);

    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER,_vboColor);

    int colorLocation = _shaders.attributeLocation("vertexColor");
    openglContext->functions()->glVertexAttribPointer(colorLocation,4,GL_FLOAT,FALSE,0, 0);
    openglContext->functions()->glEnableVertexAttribArray(colorLocation);



//    if ( _indices[i]._geomtype == itLINE){
//        glDrawArrays(GL_LINE_STRIP,_indices[i]._start,_indices[i]._count);
//    }
    if ( _indices[0]._geomtype == itLINE ){
        glDrawArrays(GL_TRIANGLE_FAN,_indices[0]._start,_indices[0]._count);
    }

    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    openglContext->functions()->glDisableVertexAttribArray(colorLocation);
    openglContext->functions()->glDisableVertexAttribArray(vertexLocation);

    _shaders.release();

     return true;
}

bool SelectionDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options, QOpenGLContext *openglContext)
{

     Size<> sz = rootDrawer()->pixelAreaSize();
    _projection.setToIdentity();
    _projection.ortho(0, 0, sz.xsize(),sz.ysize(), -1, 1);
    _mvp = _model * _view * _projection;

    if (!SimpleDrawer::prepare(prepType, options, openglContext)){
        return false;
    }

    if(!initGeometry(openglContext, _vertices, _colors))
        return false;

    qDebug() << _vertices[0]._x << _vertices[0]._y << _vertices[1]._x << _vertices[1]._y << _vertices[2]._x << _vertices[2]._y << _vertices[3]._x << _vertices[3]._y;


    return true;
}

void SelectionDrawer::unprepare(DrawerInterface::PreparationType prepType)
{
    if ( hasType(prepType, DrawerInterface::ptMVP))    {
        _prepared &= ~ ptMVP;
    }
}

bool SelectionDrawer::isPrepared(quint32 type) const
{
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
