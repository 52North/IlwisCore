#include "kernel.h"
#include "ilwisdata.h"
#include "basedrawer.h"
#include "coordinatesystem.h"
#include "rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

BaseDrawer::BaseDrawer(const QString& nme, DrawerInterface *parentDrawer, RootDrawer *rootdrawer, QObject *parent) : DrawerInterface(parent),Identity(nme), _rootDrawer(rootdrawer), _parentDrawer(parentDrawer)
{
}

void BaseDrawer::valid(bool yesno)
{
    _valid = yesno;
}

bool BaseDrawer::prepare(DrawerInterface::PreparationType,  const IOOptions &)
{
    return true;
}

void BaseDrawer::unprepare(DrawerInterface::PreparationType )
{
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

void BaseDrawer::cleanUp()
{

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




