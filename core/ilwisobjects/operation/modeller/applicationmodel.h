#ifndef APPLICATIONSETUP_H
#define APPLICATIONSETUP_H
#include "kernel_global.h"

namespace Ilwis {

class Model;
typedef IlwisData<Model> IModel;

class KERNELSHARED_EXPORT ModelApplication : public Identity
{
public:
    ModelApplication();

    void store(QDataStream& stream);
    void load(QDataStream& stream);
    virtual QString type() const = 0;
    QString attachedAnalysis() const;
    void attachedAnalysis(const QString& name);
    void attachedModel(quint64 modelid);
    IModel attachedModel();

protected:
    QString _analysisName; // every app is tied to a certain analysis
    quint64 _modelId; // the pattern maybe attached before the IModel realy exists (during prepare when a model is created)
    IModel _attachedModel;
};

typedef std::shared_ptr<ModelApplication> SPModelApplication;
}

#endif // APPLICATIONSETUP_H
