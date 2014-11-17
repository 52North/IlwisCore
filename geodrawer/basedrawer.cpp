#include "kernel.h"
#include "basedrawer.h"
#include "rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

BaseDrawer::BaseDrawer(const QString& nme, DrawerInterface *parentDrawer, RootDrawer *rootdrawer) : Identity(nme), _rootDrawer(rootdrawer), _parentDrawer(parentDrawer)
{
}

bool BaseDrawer::prepare(DrawerInterface::PreparationType,  const IOOptions &)
{
    return true;
}

bool BaseDrawer::draw(const IOOptions &) const
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



