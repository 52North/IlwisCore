#ifndef SMCESPATIALALTERNATIVES_H
#define SMCESPATIALALTERNATIVES_H

#include "workflow/applicationmodelui.h"

class SMCESpatialAlternatives : public ApplicationModelUI
{
public:
    SMCESpatialAlternatives();
    SMCESpatialAlternatives(Ilwis::ApplicationModel *app, QObject *parent);
    static ApplicationModelUI *create(Ilwis::ApplicationModel *app, QObject *parent);
    Q_INVOKABLE bool execute(const QVariantMap& inputParameters, QVariantMap& outputParameters) ;

    NEW_APPMODEL
};

#endif // SMCESPATIALALTERNATIVES_H
