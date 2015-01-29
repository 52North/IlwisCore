#include "coverage.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "colorlookup.h"
#include "representation.h"
#include "attributevisualproperties.h"
#include "table.h"
#include "layerdrawer.h"
#include "drawingcolor.h"
#include <QtGui/QOpenGLContext>
#include "drawerfactory.h"
#include "rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

LayerDrawer::LayerDrawer(const QString& name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer) : SpatialDataDrawer(name, parentDrawer, rootdrawer)
{
    valid(true); //TODO: this must move to the prepare once the correct call sequences is in place
}

bool LayerDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options, QOpenGLContext *openglContext)
{
    if(!initShaders())
        return false;

    if ( hasType(prepType, DrawerInterface::ptMVP) && !isPrepared(DrawerInterface::ptMVP)){

        _shaders.bind();
        _shaders.setUniformValue("mvp",rootDrawer()->mvpMatrix());
        _shaders.enableAttributeArray("mvp");

        _prepared |= DrawerInterface::ptMVP;
        _shaders.release();
    }

    return SpatialDataDrawer::prepare(prepType, options);

}

void LayerDrawer::unprepare(DrawerInterface::PreparationType prepType)
{
    SpatialDataDrawer::unprepare(prepType);

    if ( hasType(prepType, DrawerInterface::ptMVP))    {
        _prepared &= ~ ptMVP;
    }
}

bool LayerDrawer::initShaders() {

    if ( _shaders.shaders().size() == 0){
        _shaders.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                         "attribute highp vec4 position;"
                                         "uniform mat4 mvp;"
                                         "attribute lowp vec4 vertexColor;"
                                         "varying lowp vec4 fragmentColor;"
                                         "void main() {"
                                         "    gl_Position =  mvp * position;"
                                         "    fragmentColor = vertexColor;"
                                         "}");
        _shaders.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                         "varying lowp vec4 fragmentColor;"
                                         "void main() {"
                                         "    gl_FragColor = fragmentColor;"
                                         "}");





        //_shaders.bindAttributeLocation("aVertices", 0);
        if(!_shaders.link()){
            return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR(_shaders.log()));
        }
        if (!_shaders.bind()){
            return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR(_shaders.log()));
        }
    }

    return true;
}

bool LayerDrawer::initGeometry(QOpenGLContext *openglContext, const std::vector<VertexPosition> &vertices,const std::vector<VertexColor>& colors) {
    if ( !openglContext){
        return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR("Invalid OpenGL context passed"));
    }

    openglContext->functions()->glGenBuffers (1, &_vboPosition);
    openglContext->functions()->glBindBuffer (GL_ARRAY_BUFFER, _vboPosition);
    openglContext->functions()->glBufferData (GL_ARRAY_BUFFER, sizeof (VertexPosition) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    openglContext->functions()->glGenBuffers (1, &_vboColor);
    openglContext->functions()->glBindBuffer (GL_ARRAY_BUFFER, _vboColor);
    openglContext->functions()->glBufferData (GL_ARRAY_BUFFER, sizeof (VertexColor) * colors.size(), &colors[0], GL_STATIC_DRAW);

    GLenum err =  glGetError();
    if ( err != 0) {
        return ERROR1(QString(TR("Drawing failed : OpenGL returned error code %1")),QString::number(err));
    }
    return true;
}

QString LayerDrawer::activeAttribute() const
{
    return _activeAttribute;
}

void LayerDrawer::attribute(const QString &key, const QVariant &attribValue)
{
    if ( key == "activevisualattribute"){
        setActiveVisualAttribute(attribValue.toString());
    }
}

QVariant LayerDrawer::attribute(const QString &key) const
{
    QVariant var = SpatialDataDrawer::attribute(key);
    if ( var.isValid())
        return var;

    if ( key == "activevisualattribute"){
        return _activeAttribute;
    }
    return QVariant();
}

QColor LayerDrawer::color(const IRepresentation &rpr, double value, DrawerInterface::ColorValueMeaning cvm)
{
    if ( _activeAttribute != sUNDEF){
        AttributeVisualProperties& attr = _visualProperties[_activeAttribute];
        if ( cvm ==DrawerInterface::cvmFRACTION){
            NumericRange numrange = attr.stretchRange();
            if ( !numrange.isValid()){
                numrange = attr.actualRange();
            }
            value = numrange.min() + numrange.distance() * value;

        }
        QColor clr = attr.value2color(value);
        return clr;
    }
    return QColor();
}

void LayerDrawer::setActiveVisualAttribute(const QString &attr)
{
    if ( !isVisualAttribute(attr)){
        return;
    }
    _activeAttribute = attr;
}

void LayerDrawer::cleanUp()
{
}




