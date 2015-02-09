#include "kernel.h"
#include "linedrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

LineDrawer::LineDrawer(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer) : SimpleDrawer(name, parentDrawer, rootdrawer, IOOptions())
{

}

bool LineDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &, QOpenGLContext *openglContext)
{
    if ( prepType == DrawerInterface::ptGEOMETRY){

    }
    return true;
}

bool LineDrawer::draw(const IOOptions &options) const
{
    if (!isActive() || !isValid())
        return false;

    if ( _coords.size() == 0)
        return false;

    if ( containment() == BaseDrawer::cOUTSIDE)
        return false;

    return true;

}
