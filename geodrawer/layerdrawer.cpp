#include "coverage.h"
#include "layerdrawer.h"
#include "drawingcolor.h"

using namespace Ilwis;
using namespace Geodrawer;

LayerDrawer::LayerDrawer(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer) : SpatialDataDrawer(name, parentDrawer, rootdrawer)
{
}

bool LayerDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options)
{
    return SpatialDataDrawer::prepare(prepType, options);
}

void LayerDrawer::setCoverage(const ICoverage &coverage)
{
    SpatialDataDrawer::setCoverage(coverage);
    // TODO initialize drawing color here
    _drawingColor.reset(new DrawingColor(this));
}

UPDrawingColor &LayerDrawer::drawingColor()
{
    return _drawingColor;
}


