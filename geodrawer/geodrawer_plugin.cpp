#include "geodrawer_plugin.h"
#include "geodrawer.h"

#include <qqml.h>

#include "geometries.h"
#include "iooptions.h"
#include "drawerfactory.h"
#include "selectiondrawer.h"

void GeodrawerPlugin::registerTypes(const char *uri)
{
    // @uri n52.org.ilwisobjects
    qmlRegisterType<GeoDrawer>(uri, 1, 0, "GeoDrawer");

    Ilwis::Geodrawer::DrawerFactory::registerDrawer("SelectionDrawer", Ilwis::Geodrawer::SelectionDrawer::create);


}


