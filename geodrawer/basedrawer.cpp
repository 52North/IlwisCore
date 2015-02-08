#include "kernel.h"
#include "ilwisdata.h"
#include "basedrawer.h"
#include "coordinatesystem.h"
#include "rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

BaseDrawer::BaseDrawer(const QString& nme, DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &) : DrawerInterface(0),Identity(nme,i64UNDEF,nme), _rootDrawer(rootdrawer), _parentDrawer(parentDrawer)
{
    Identity::prepare();
}

void BaseDrawer::valid(bool yesno)
{
    _valid = yesno;
}

bool BaseDrawer::prepare(DrawerInterface::PreparationType prepType,  const IOOptions &, QOpenGLContext *)
{
    if ( code() == "RootDrawer") // rootdrawer for the moment has no need of shaders
        return true;

    if ( !rootDrawer()) // rootdrawer must be set
        return false;

    if ( hasType(prepType, ptSHADERS) && !isPrepared(ptSHADERS)){
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

        if(!_shaders.link()){
            return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR(_shaders.log()));
        }
        if (!_shaders.bind()){
            return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR(_shaders.log()));
        }
        _prepared |= DrawerInterface::ptSHADERS;

    }


    return true;
}

bool BaseDrawer::moveGeometry2GPU(QOpenGLContext *openglContext, const std::vector<VertexPosition> &vertices,const std::vector<VertexColor>& colors) {
    if ( !openglContext){
        return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR("Invalid OpenGL context passed"));
    }
    if ( _vboPosition != iUNDEF)
        openglContext->functions()->glDeleteBuffers(1,&_vboPosition);
    if ( _vboColor != iUNDEF)
        openglContext->functions()->glDeleteBuffers(1,&_vboColor);

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

void BaseDrawer::unprepare(DrawerInterface::PreparationType prepType )
{
    if ( hasType(_prepared, prepType))    {
        _prepared &= ~ prepType;
    }
}

bool BaseDrawer::isPrepared(quint32 type) const
{
    return hasType(_prepared, type);
}

bool BaseDrawer::draw(QOpenGLContext *, const IOOptions &) const
{
    return false;
}

RootDrawer *BaseDrawer::rootDrawer()
{
    return _rootDrawer;
}

const RootDrawer *BaseDrawer::rootDrawer() const
{
    return _rootDrawer;
}

DrawerInterface *BaseDrawer::parent()
{
    return _parentDrawer;
}

const DrawerInterface *BaseDrawer::parent() const
{
    return _parentDrawer;
}

bool BaseDrawer::isActive() const
{
    return _active;
}

void BaseDrawer::active(bool yesno)
{
    _active = yesno;
}

bool BaseDrawer::isValid() const
{
    return _valid;
}

void BaseDrawer::selected(bool yesno)
{
    _selected= yesno;
}

bool BaseDrawer::isSelected() const
{
    return _selected;
}

BaseDrawer::Containment BaseDrawer::containment() const
{
    if ( _envelope.isValid()){
        if ( rootDrawer()->viewEnvelope().intersects(_envelope))
            return BaseDrawer::cINSIDE;
    }
    return BaseDrawer::cUNKNOWN;
}

void BaseDrawer::cleanUp(QOpenGLContext *openglContext)
{
    if ( _vboPosition != iUNDEF)
        openglContext->functions()->glDeleteBuffers(1,&_vboPosition);
    if ( _vboColor != iUNDEF)
        openglContext->functions()->glDeleteBuffers(1,&_vboColor);

    _vboPosition = _vboColor = iUNDEF;
}

void BaseDrawer::code(const QString &code)
{
    Identity::code(code);
}

QString BaseDrawer::code() const
{
    return Identity::code();
}

quint64 BaseDrawer::id() const
{
    return Identity::id();
}

QString BaseDrawer::name() const
{
    return Identity::name();
}

void BaseDrawer::name(const QString &n)
{
    Identity::name(n);
}

QString BaseDrawer::description() const
{
    return Identity::description();
}

void BaseDrawer::setDescription(const QString &desc)
{
    return Identity::setDescription(desc);
}

std::vector<QVariant> BaseDrawer::attributes(const QString &attrNames) const
{
    std::vector<QVariant> result;
    return result;
}

QVariant BaseDrawer::attribute(const QString &attrName) const
{
    return QVariant();
}

void BaseDrawer::setAttribute(const QString &, const QVariant &)
{

}

bool BaseDrawer::drawerAttribute(const QString , const QString &, const QVariant &)
{
    return false;
}

QColor BaseDrawer::color(const IRepresentation &rpr, double , DrawerInterface::ColorValueMeaning )
{
    return QColor();
}

quint32 BaseDrawer::defaultOrder() const
{
    return iUNDEF;
}





