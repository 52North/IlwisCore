#ifndef SMCEMODEL_H
#define SMCEMODEL_H

#include <QObject>
#include "workflow/analysismodel.h"

class SMCEModel : public AnalysisModel
{
    Q_OBJECT
public:
    SMCEModel();
    SMCEModel(Ilwis::AnalysisPattern *p);
    static AnalysisModel *create(Ilwis::AnalysisPattern *pattern);

private:

    NEW_ANALYSISMODEL



};

#endif // SMCEMODEL_H
