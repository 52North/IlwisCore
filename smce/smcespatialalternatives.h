#ifndef SMCESPATIALALTERNATIVES_H
#define SMCESPATIALALTERNATIVES_H

#include "workflow/applicationmodel.h"

class SMCESpatialAlternatives : public ApplicationModel
{
public:
    SMCESpatialAlternatives();
    SMCESpatialAlternatives(Ilwis::ModelApplication *app);
    static Ilwis::ModelApplication *create(Ilwis::ModelApplication *app);
    Q_INVOKABLE bool execute(const QVariantMap& inputParameters, QVariantMap& outputParameters) ;
};

#endif // SMCESPATIALALTERNATIVES_H
