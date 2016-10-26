#ifndef ANALYSISPATTERN_H
#define ANALYSISPATTERN_H
#include "kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT AnalysisPattern : public Identity
{
public:
    AnalysisPattern();
    AnalysisPattern(const QString& name, const QString& description=sUNDEF);

    void store(QDataStream& stream);
    void load(QDataStream& stream);
};

typedef std::shared_ptr<AnalysisPattern> SPAnalysisPattern;
}

#endif // ANALYSISPATTERN_H
