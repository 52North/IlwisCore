#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/modellerfactory.h"
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
#include "modelsmcespatialalternatives.h"


using namespace Ilwis;
using namespace Smce;

REGISTER_MODELAPPLICATION(SMCESpatialAlternativesModel,"smcespatialalternatives")

SMCESpatialAlternativesModel::SMCESpatialAlternativesModel() :
    ApplicationModel("smcespatialalternatives",TR("calculates the result(s) of a specific smce for different areas"))
{
   attachedAnalysis("d4"); // a test name
}

void SMCESpatialAlternativesModel::store(QDataStream &stream)
{
   stream << type();
   ApplicationModel::store(stream);
}

void SMCESpatialAlternativesModel::loadMetadata(QDataStream &stream)
{
    ApplicationModel::load(stream);
}

void SMCESpatialAlternativesModel::loadData(QDataStream &stream)
{
    //TODO
}

QString SMCESpatialAlternativesModel::type() const
{
    return "smcespatialalternatives";
}

ApplicationModel *SMCESpatialAlternativesModel::create()
{
    return new SMCESpatialAlternativesModel()    ;
}
