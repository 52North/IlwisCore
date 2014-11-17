#ifndef FEATURELAYERDRAWER_H
#define FEATURELAYERDRAWER_H

#include "layerdrawer.h"

namespace Ilwis {
namespace Geodrawer{


class FeatureLayerDrawer : public LayerDrawer
{
public:
    FeatureLayerDrawer(const QString &name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer);
};
}
}

#endif // FEATURELAYERDRAWER_H
