#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "modeller/applicationmodel.h"
#include "modeller/model.h"
#include "workflow/modelbuilder.h"
#include "workflow/applicationmodelui.h"
#include "smcemodel.h"
#include "smcespatialalternatives.h"

REGISTER_APPMODEL(SMCESpatialAlternatives, "smce")

using namespace Ilwis;

SMCESpatialAlternatives::SMCESpatialAlternatives()
{

}

SMCESpatialAlternatives::SMCESpatialAlternatives(ApplicationModel *app, QObject *parent) : ApplicationModelUI(app, parent)
{
    _panels["main"] = "SMCE/SMCESpatialAlternativePanel.qml";
    _panels["form"] = "SMCE/SMCEAppForm.qml";

}

ApplicationModelUI *SMCESpatialAlternatives::create(ApplicationModel *app, QObject *parent)
{
    return new SMCESpatialAlternatives(app, parent);
}

bool SMCESpatialAlternatives::execute(const QVariantMap &inputParameters, QVariantMap &outputParameters)
{
    return false;
}
