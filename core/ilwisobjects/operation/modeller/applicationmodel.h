#ifndef APPLICATIONSETUP_H
#define APPLICATIONSETUP_H
#include "kernel_global.h"

#define NEW_MODELAPPLICATION \
    private: \
static Ilwis::ApplicationModel *dummy_model;

#define REGISTER_MODELAPPLICATION(classname,modelname) \
    Ilwis::ApplicationModel *classname::dummy_model = ModellerFactory::registerModelApplication(modelname, classname::create);

namespace Ilwis {

class Model;
typedef IlwisData<Model> IModel;

class KERNELSHARED_EXPORT ApplicationModel : public Identity
{
public:
    ApplicationModel();
    ApplicationModel(const QString &name, const QString &description);

    virtual void store(QDataStream& stream);
    virtual void loadData(QDataStream& stream){} // empty implementation as it is often not needed
    virtual void loadMetadata(QDataStream& stream);
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

typedef std::shared_ptr<ApplicationModel> SPModelApplication;
}

#endif // APPLICATIONSETUP_H
