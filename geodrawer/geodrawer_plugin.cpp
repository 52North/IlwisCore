#include "geodrawer_plugin.h"
#include "geodrawer.h"

#include <qqml.h>

void GeodrawerPlugin::registerTypes(const char *uri)
{
    // @uri n52.org.ilwisobjects
    qmlRegisterType<GeoDrawer>(uri, 1, 0, "GeoDrawer");
}


