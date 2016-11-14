#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/modellerfactory.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "smce.h"

using namespace Ilwis;
using namespace Smce;

REGISTER_ANALYSISPATTERN(SMCE)

SMCE::SMCE() : Ilwis::AnalysisPattern()
{

}

bool SMCE::execute(const QVariantMap &inputParameters, QVariantMap &outputParameters)
{
    return false;
}

IWorkflow SMCE::workflow(const IOOptions &opt)
{
    return IWorkflow();
}

void SMCE::store(QDataStream &stream)
{
    stream << type();
    AnalysisPattern::store(stream);
    stream << _test;
}

void SMCE::load(QDataStream &stream)
{
    AnalysisPattern::load(stream);
    stream >> _test;
}

QString SMCE::type() const
{
    return "smce";
}

SMCE::SMCE(const QString &name, const QString &description) : AnalysisPattern(name, description){

}

SMCE::AnalysisPattern *SMCE::create(const QString &name, const QString &description, const IOOptions &options)
{
    SMCE *smce =  new SMCE(name, description);
    return smce;
}


