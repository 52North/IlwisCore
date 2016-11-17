#ifndef MODELSMCESPATIALALTERNATIVES_H
#define MODELSMCESPATIALALTERNATIVES_H

#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/workflow.h"
#include "modeller/applicationmodel.h"

namespace Ilwis{
class ApplicationModel;

namespace Smce {
class SMCESpatialAlternativesModel : public ApplicationModel
{
public:
    SMCESpatialAlternativesModel();
    void store(QDataStream& stream);
    void loadMetadata(QDataStream& stream);
    virtual void loadData(QDataStream& stream);
    QString type() const;
    static ApplicationModel *create();

NEW_MODELAPPLICATION
};
}
}

#endif // MODELSMCESPATIALALTERNATIVES_H
