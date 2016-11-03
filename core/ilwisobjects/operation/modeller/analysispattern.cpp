#include "kernel.h"
#include "ilwisdata.h"
#include "workflow.h"
#include "analysispattern.h"

using namespace Ilwis;

AnalysisPattern::AnalysisPattern()
{

}

AnalysisPattern::AnalysisPattern(const QString &name, const QString &description) : Identity(name,i64UNDEF, description)
{

}

void AnalysisPattern::store(QDataStream &stream)
{
    Identity::store(stream)    ;
}

void AnalysisPattern::load(QDataStream &stream)
{
    Identity::store(stream)    ;
}
