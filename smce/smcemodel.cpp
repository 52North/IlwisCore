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
#include "smcemodel.h"
#include "smce.h"

REGISTER_ANALYSISMODEL(SMCEModel,"smce")

SMCEModel::SMCEModel()
{

}

SMCEModel::SMCEModel(Ilwis::AnalysisPattern *p) : AnalysisModel(p){
    _panels["main"] = "SMCE/SMCEPanel.qml";
    _panels["form"] = "SMCE/SMCEForm.qml";
}

AnalysisModel *SMCEModel::create(Ilwis::AnalysisPattern *pattern)
{
    return new SMCEModel(pattern);
}

bool SMCEModel::execute(const QVariantMap inputParameters, QVariantMap outputParameters)
{
    if ( _analysis){
        return _analysis->execute(inputParameters, outputParameters);
    }
    return false;
}

Node* SMCEModel::tree() const
{
    return ((Ilwis::Smce::SMCE*)_analysis)->root();
}
