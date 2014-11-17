#include "coverage.h"
#include "drawingcolor.h"
#include "featurelayerdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

FeatureLayerDrawer::FeatureLayerDrawer(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer) : LayerDrawer(name, parentDrawer, rootdrawer)
{
}
