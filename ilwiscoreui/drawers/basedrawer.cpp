#include "kernel.h"
#include "ilwisdata.h"
#include "basedrawer.h"
#include "coordinatesystem.h"
#include "rootdrawer.h"
#include "ilwiscontext.h"

using namespace Ilwis;
using namespace Geodrawer;

BaseDrawer::BaseDrawer(const QString& nme, DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &) : DrawerInterface(0),Identity(nme,i64UNDEF,nme.toLower()), _rootDrawer(rootdrawer), _parentDrawer(parentDrawer)
{
    Identity::prepare();
}

void BaseDrawer::valid(bool yesno)
{
    _valid = yesno;
}

bool BaseDrawer::prepare(DrawerInterface::PreparationType prepType,  const IOOptions &)
{
    if ( code() == "RootDrawer") // rootdrawer for the moment has no need of shaders
        return true;

    if ( !rootDrawer()) // rootdrawer must be set
        return false;

    if ( hasType(prepType, ptSHADERS) && !isPrepared(ptSHADERS)){
        QString ilwisloc = context()->ilwisFolder().absoluteFilePath();
        QString vertexglsllocation = ilwisloc + "/resources/" + _vertexShader;
        _shaders.addShaderFromSourceFile(QOpenGLShader::Vertex,vertexglsllocation);
        QString fragmentgslslocation = ilwisloc + "/resources/" + _fragmentShader;
        _shaders.addShaderFromSourceFile(QOpenGLShader::Fragment,fragmentgslslocation);

        if(!_shaders.link()){
            return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR(_shaders.log()));
        }
        if (!_shaders.bind()){
            return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR(_shaders.log()));
        }

        _vboPosition = _shaders.attributeLocation("position");
        _vboNormal = _shaders.attributeLocation("normal");
        _modelview = _shaders.uniformLocation("mvp");
        _vboAlpha = _shaders.uniformLocation("alpha");
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

bool BaseDrawer::draw(const IOOptions &) const
{
    return false;
}

std::unique_ptr<DrawerInterface> &BaseDrawer::drawer(const QString &, DrawerInterface::DrawerType )
{
    return _dummy;
}

const std::unique_ptr<DrawerInterface> &BaseDrawer::drawer(const QString &, DrawerInterface::DrawerType ) const
{
    return _dummy;
}

RootDrawer *BaseDrawer::rootDrawer()
{
    return _rootDrawer;
}

const RootDrawer *BaseDrawer::rootDrawer() const
{
    if (!_rootDrawer) // this is the rootdrawer
        return static_cast<const RootDrawer *>(this);
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
        if ( rootDrawer()->zoomEnvelope().intersects(_envelope))
            return BaseDrawer::cINSIDE;
    }
    return BaseDrawer::cUNKNOWN;
}

void BaseDrawer::cleanUp()
{
    _shaders.removeAllShaders();
    unprepare(ptSHADERS);
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
    if ( attrName == "opacity")
        return _alpha;
    if ( attrName == "active")
        return _active;

    return QVariant();
}

QVariant BaseDrawer::attributeOfDrawer(const QString &, const QString &) const
{
    return QVariant();
}

void BaseDrawer::setAttribute(const QString &attrName, const QVariant &value)
{
    if ( attrName == "opacity")
        _alpha = value.toFloat();
    if ( attrName == "active")
        _active= value.toBool();
}

bool BaseDrawer::drawerAttribute(const QString& , const QString &, const QVariant &)
{
    return false;
}

void BaseDrawer::resetVisualProperty(const QString &propertyName, const IRepresentation &rpr)
{

}

QVariant BaseDrawer::execute(const QString &operationName, const QVariantMap &parameters)
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

float BaseDrawer::alpha() const
{
    return _alpha;
}

void BaseDrawer::alpha(float alp)
{
    if ( alp >= 0 && alp <= 1.0)
        _alpha = alp;
}

void BaseDrawer::redraw()
{
    rootDrawer()->redraw();
}





