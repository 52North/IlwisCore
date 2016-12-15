#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "modeller/applicationmodel.h"
#include "modeller/model.h"
#include "workflow/modelbuilder.h"
#include "smcemodel.h"


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

bool SMCEModel::execute(const QVariantMap &inputParameters, QVariantMap &outputParameters)
{

    // execute the tree calculation here:
    // create the python script and run it using "//operations/runpython"

    if ( _analysis){
        return _analysis->execute(inputParameters, outputParameters);
    }
    return false;
}
