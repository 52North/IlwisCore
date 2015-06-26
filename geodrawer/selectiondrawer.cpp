#include "geometries.h"
#include "iooptions.h"
#include "drawers/drawerfactory.h"
#include "rootdrawer.h"
#include "selectiondrawer.h"
#include "boost/math/special_functions/sign.hpp"

using namespace Ilwis;
using namespace Geodrawer;

//REGISTER_DRAWER(SelectionDrawer)

SelectionDrawer::SelectionDrawer(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) : SimpleDrawer("SelectionDrawer", parentDrawer,rootdrawer, options)
{
    _vertexShader = "featurevertexshader_nvdia.glsl";
    _fragmentShader = "featurefragmentshader_nvdia.glsl";

    QColor clr;
    _colors.resize(13);
    if ( options.contains("areacolor")){
        clr = options["areacolor"].value<QColor>();
    }else {
        clr = QColor(80,80,0);
        clr.setAlphaF(0.15);
    }
    _colors[9] = _colors[10] = _colors[11] = _colors[12] = clr;
    clr.setAlphaF(0.5);
    _colors[5] = _colors[6] = _colors[7] = _colors[8] = clr;

    if ( options.contains("bordercolor")){
        clr = options["bordercolor"].value<QColor>();
    }else {
        clr = QColor(0,0,0);
        clr.setAlphaF(1);
    }
    _colors[0] = _colors[1] = _colors[2] = _colors[3] = _colors[4] = clr;
    QVector3D pos(0,0,0);
    _vertices = { pos,pos,pos,pos,pos, pos,pos,pos,pos, pos,pos,pos,pos};

    _indices.push_back(VertexIndex(0, 5, itLINE, iUNDEF));
    _indices.push_back(VertexIndex(5, 2, itLINE, iUNDEF));
    _indices.push_back(VertexIndex(7, 2, itLINE, iUNDEF));
    _indices.push_back(VertexIndex(9, 4, itPOLYGON, iUNDEF));
}

SelectionDrawer::~SelectionDrawer()
{

}

bool SelectionDrawer::draw(const IOOptions &)
{

    if ( !isActive())
        return false;

    if (!isPrepared()){
        return false;
    }

    if (!_shaders.bind())
        return false;
     _shaders.setUniformValue(_modelview,_mvp);
     _shaders.setUniformValue(_vboAlpha, alpha());

    _shaders.enableAttributeArray(_vboPosition);
    _shaders.enableAttributeArray(_vboNormal);
    _shaders.enableAttributeArray(_vboColor);

    _shaders.setAttributeArray(_vboPosition,_vertices.constData());
    _shaders.setAttributeArray(_vboNormal, _normals.constData());
    _shaders.setAttributeArray(_vboColor, GL_FLOAT, (void *)_colors.data(),4);
    _shaders.setUniformValue(_scaleFactor, 1.0f);

    glDrawArrays(GL_LINE_STRIP,_indices[0]._start,_indices[0]._count);
    glDrawArrays(GL_LINE_STRIP,_indices[1]._start,_indices[1]._count);
    glDrawArrays(GL_LINE_STRIP,_indices[2]._start,_indices[2]._count);
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    glDrawArrays(GL_TRIANGLE_FAN,_indices[3]._start,_indices[3]._count);
    glDisable(GL_COLOR_LOGIC_OP);

    _shaders.disableAttributeArray(_vboPosition);
    _shaders.disableAttributeArray(_vboNormal);
    _shaders.disableAttributeArray(_vboColor);

    _shaders.release();


     return true;
}

bool SelectionDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options)
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

    if ( hasType(prepType, DrawerInterface::ptMVP) && !isPrepared(DrawerInterface::ptMVP) ){

        if (!_shaders.bind())
            return false;

        Size<> sz = rootDrawer()->pixelAreaSize();
        _projection.setToIdentity();
        _projection.ortho(QRect(0,0,sz.xsize(), sz.ysize()));
        _mvp = _model * _view * _projection;



        _prepared |= DrawerInterface::ptMVP;
        _shaders.release();
    }

    return true;
}

void SelectionDrawer::setVertices(float x1, float y1, float x2, float y2)
{
    _vertices[0] = {x1,y1,0};
    _vertices[1] = {x2,y1,0};
    _vertices[2] = {x2,y2,0};
    _vertices[3] = {x1,y2,0};
    _vertices[4] = {x1,y1,0};
    _vertices[9] = {x1,y1,0};
    _vertices[10] = {x2,y1,0};
    _vertices[11] = {x2,y2,0};
    _vertices[12] = {x1,y2,0};
}

void SelectionDrawer::setAttribute(const QString &attrName, const QVariant &attrib)
{
    if ( attrName == "preserveaspectration"){
        _preserveAspectRatio = attrib.toBool();
    }
    if ( attrName == "boundingbox"){
        QStringList parts = attrib.toString().split(" ");
        bool ok1, ok2, ok3, ok4;
        float x1,x2,y1,y2;
        if ( parts.size() == 4){
            x1 = parts[0].toFloat(&ok1);
            y1 = parts[1].toFloat(&ok2);
            x2 = parts[2].toFloat(&ok3);
            y2 = parts[3].toFloat(&ok4);
        }else if ( parts.size() == 6){
            x1 = parts[0].toFloat(&ok1);
            y1 = parts[1].toFloat(&ok2);
            x2 = parts[3].toFloat(&ok3);
            y2 = parts[4].toFloat(&ok4);
        }
        if ( ok1 && ok2 && ok3 && ok4){
            setVertices(y1, x1, y2, x2);
        }
    }
    if ( attrName == "envelope"){
        QStringList parts = attrib.toString().split(" ");
        Coordinate crd1, crd2;
        bool ok1 = false, ok2=false, ok3 = false, ok4 = false;
        if ( parts.size() == 4){
            crd1 = Coordinate(parts[0].toFloat(&ok1), parts[1].toFloat(&ok2));
            crd2 = Coordinate(parts[2].toFloat(&ok3), parts[3].toFloat(&ok4));
        }else   if ( parts.size() == 6){
            crd1 = Coordinate(parts[0].toFloat(&ok1), parts[1].toFloat(&ok2));
            crd2 = Coordinate(parts[3].toFloat(&ok3), parts[4].toFloat(&ok4));
        }
        if (ok1 && ok2 && ok3 && ok4){
            Size<> sz = rootDrawer()->pixelAreaSize();
            Pixel pix1 = rootDrawer()->coord2Pixel(crd1);
            Pixel pix2 = rootDrawer()->coord2Pixel(crd2);
            setVertices(pix1.x,sz.ysize() -pix1.y, pix2.x, sz.ysize() - pix2.y);
        }
    }

    if ( attrName == "currentx"){
        float value = attrib.toFloat();
        _vertices[1].setX(value);
        _vertices[2].setX(value);
        _vertices[10].setX(value);
        _vertices[11].setX(value);


    }
    if ( attrName == "currenty"){
        float value = attrib.toFloat();
        value = rootDrawer()->pixelAreaSize().ysize() - value;
        _vertices[2].setY(value);
        _vertices[3].setY(value);
        _vertices[11].setY(value);
        _vertices[12].setY(value);

    }
    if ( _preserveAspectRatio){
        double aspectRatioView = rootDrawer()->aspectRatioView();
        double dx = _vertices[1].x() - _vertices[0].x();
        double dy = _vertices[3].y() - _vertices[0].y();
        if ( aspectRatioView <= 1){
             _vertices[3].setY(_vertices[0].y() + boost::math::sign(dy) * dx/ aspectRatioView);
             _vertices[11].setY(_vertices[3].y());
             _vertices[12].setY(_vertices[3].y());
             _vertices[2].setY(_vertices[3].y());

        }else {
            _vertices[1].setX(_vertices[0].x() + boost::math::sign(dx) * dy * aspectRatioView);
            _vertices[11].setX(_vertices[1].x());
            _vertices[10].setX(_vertices[1].x());
            _vertices[2].setX(_vertices[1].x());
        }
    }
    if ( attrName == "initialx"){
        bool ok;
        double x = attrib.toFloat(&ok);
        if (!ok)
            x = flUNDEF;
        for(int i = 0; i < 13; ++i)
            _vertices[i][0] = x;
    }
    if ( attrName == "initialy"){
        bool ok;
        double y = attrib.toFloat(&ok);
        if (!ok)
            y = flUNDEF;
        y = rootDrawer()->pixelAreaSize().ysize() - y;
        for(int i = 0; i < 13; ++i)
            _vertices[i][1] = y;
    }
    _vertices[8].setY((_vertices[0].y() + _vertices[2].y()) /2);
    _vertices[7].setY(_vertices[8].y());
    _vertices[6].setY(_vertices[2].y());
    _vertices[6].setX((_vertices[0].x() + _vertices[1].x()) /2);
    _vertices[5].setX(_vertices[6].x());
    _vertices[8].setX(_vertices[1].x());
    unprepare(ptGEOMETRY);
}

bool SelectionDrawer::drawerAttribute(const QString drawercode, const QString &attrName, const QVariant &attrib)
{
    if (drawercode != code())
        return false;
    setAttribute(attrName, attrib);

    return true;
}

QVariant SelectionDrawer::attribute(const QString &attrName) const
{
    if ( attrName == "boundingbox"){
        return qVariantFromValue(BoundingBox(Pixel(_vertices[0].x(),_vertices[0].y()),Pixel(_vertices[2].x(),_vertices[2].y())));
    }
    if ( attrName == "envelope"){
        return qVariantFromValue(envelope());
    }
    return QVariant();
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
    QVector3D v1( _vertices[0].x(), _vertices[0].y(), 0);
    QVector3D v2( _vertices[2].x(), _vertices[2].y(), 0);
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

void SelectionDrawer::cleanUp()
{
    SimpleDrawer::cleanUp();
   // rootDrawer()->applyEnvelopeZoom(envelope());
}


