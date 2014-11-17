#ifndef LAYERDRAWER_H
#define LAYERDRAWER_H

#include "spatialdatadrawer.h"

namespace Ilwis {
namespace Geodrawer{

class DrawingColor;
typedef std::unique_ptr<DrawingColor> UPDrawingColor;

class LayerDrawer : public SpatialDataDrawer
{
public:
    LayerDrawer(const QString &name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer);

    bool prepare(PreparationType prepType, const IOOptions& options);
    void setCoverage(const ICoverage& coverage);

    UPDrawingColor& drawingColor();

private:
    UPDrawingColor _drawingColor;
};


}
}

#endif // LAYERDRAWER_H
