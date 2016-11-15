#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "modeller/applicationsetup.h"
#include "modeller/model.h"
#include "workflow/modelbuilder.h"
#include "smcemodel.h"
#include "smcespatialalternatives.h"

SMCESpatialAlternatives::SMCESpatialAlternatives()
{

}

SMCESpatialAlternatives::SMCESpatialAlternatives(Ilwis::ModelApplication *app): ApplicationModel(app)
{
    _panels["main"] = "SMCE/SMCESpatialAlternativePanel.qml";
}

Ilwis::ModelApplication *SMCESpatialAlternatives::create(Ilwis::ModelApplication *app)
{
    return 0;
}

bool SMCESpatialAlternatives::execute(const QVariantMap &inputParameters, QVariantMap &outputParameters)
{
    return false;
}
