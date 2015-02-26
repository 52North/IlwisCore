#include "geodrawer_plugin.h"
#include "visualizationmanager.h"
//#include "geodrawer.h"

#include <qqml.h>

#include "geometries.h"
#include "iooptions.h"
#include "drawers/drawerfactory.h"
#include "selectiondrawer.h"
#include "layersview.h"

void GeodrawerPlugin::registerTypes(const char *uri)
{
    // @uri n52.org.ilwisobjects
    qmlRegisterType<LayersView>(uri, 1, 0, "LayersView");

    Ilwis::Geodrawer::DrawerFactory::registerDrawer("SelectionDrawer", Ilwis::Geodrawer::SelectionDrawer::create);


}


