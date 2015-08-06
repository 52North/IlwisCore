#ifndef GLOBALLAYERMODEL_H
#define GLOBALLAYERMODEL_H

#include "coveragelayermodel.h"

class GlobalLayerModel : public CoverageLayerModel
{
public:
    GlobalLayerModel();
    GlobalLayerModel(LayerManager *parent);

    Ilwis::Geodrawer::DrawerInterface *drawer();
    const Ilwis::Geodrawer::DrawerInterface *drawer() const;

private:
    LayerManager *_layermanager = 0;


};

#endif // GLOBALLAYERMODEL_H
