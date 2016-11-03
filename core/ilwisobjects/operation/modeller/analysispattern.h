#ifndef ANALYSISPATTERN_H
#define ANALYSISPATTERN_H
#include "kernel_global.h"

namespace Ilwis {
class Workfolow;
typedef IlwisData<Workflow> IWorkflow;

class KERNELSHARED_EXPORT AnalysisPattern : public Identity
{
public:
    AnalysisPattern();
    AnalysisPattern(const QString& name, const QString& description=sUNDEF);

    virtual bool execute(const QVariantMap& inputParameters, QVariantMap& outputParameters)  = 0;
    virtual IWorkflow workflow(const IOOptions& opt) = 0;

    virtual void store(QDataStream& stream) = 0;
    virtual void load(QDataStream& stream) = 0;
};

typedef std::shared_ptr<AnalysisPattern> SPAnalysisPattern;
}

#endif // ANALYSISPATTERN_H
