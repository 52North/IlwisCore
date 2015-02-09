#include "kernel.h"
#include "ilwisdata.h"
#include "simpledrawer.h"
#include "rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

SimpleDrawer::SimpleDrawer(const QString& nme, DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) : BaseDrawer(nme, parentDrawer, rootdrawer, options)
{
}

bool SimpleDrawer::isSimple() const
{
    return true;
}




