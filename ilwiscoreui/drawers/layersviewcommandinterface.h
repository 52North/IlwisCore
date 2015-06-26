#ifndef LAYERSVIEWCOMMANDINTERFACE
#define LAYERSVIEWCOMMANDINTERFACE

#include "ilwiscoreui_global.h"
#include "drawerinterface.h"
#include <QVariantMap>

namespace Ilwis {
class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

}
class LayerManager;

class ILWISCOREUISHARED_EXPORT LayersViewCommandInterface {
public:
    LayersViewCommandInterface();
    virtual ~LayersViewCommandInterface();
    virtual void addCommand(const QString& command, const QVariantMap& params) = 0;
    virtual LayerManager *layerManager() = 0;
    virtual Ilwis::Geodrawer::DrawerInterface *rootDrawer() const = 0;
};

#endif // LAYERSVIEWCOMMANDINTERFACE

