#include "kernel.h"
#include "ilwisdata.h"
#include "simpledrawer.h"
#include "rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

SimpleDrawer::SimpleDrawer(const QString& nme, DrawerInterface *parentDrawer, RootDrawer *rootdrawer) : BaseDrawer(nme, parentDrawer, rootdrawer)
{
}

bool SimpleDrawer::isSimple() const
{
    return true;
}

std::vector<VertexPosition> &SimpleDrawer::drawPositions()
{
    return _positions;
}


