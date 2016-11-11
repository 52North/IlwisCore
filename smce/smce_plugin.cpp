#include "smce_plugin.h"
//#include "geodrawer.h"

#include <qqml.h>

#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/modellerfactory.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "geometries.h"
#include "iooptions.h"
#include "drawers/selectiondrawer.h"
#include "smcemodel.h"

void SMCEPlugin::registerTypes(const char *uri)
{
    // @uri n52.org.ilwisobjects
    qmlRegisterType<SMCEModel>(uri, 1, 0, "SMCE");

    Ilwis::kernel()->issues()->log("Loaded module SMCE",Ilwis::IssueObject::itMessage);
}


