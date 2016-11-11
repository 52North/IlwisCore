#ifndef ANALYSISPATTERN_H
#define ANALYSISPATTERN_H
#include "kernel_global.h"

#define NEW_ANALYSISPATTERN \
    private: \
static Ilwis::AnalysisPattern *dummy_analysis;

#define REGISTER_ANALYSISPATTERN(classname) \
    AnalysisPattern *classname::dummy_analysis = ModellerFactory::registerAnalysisPattern(#classname, classname::create);

namespace Ilwis {
class Workfolow;
typedef IlwisData<Workflow> IWorkflow;
class Model;
typedef IlwisData<Model> IModel;

class KERNELSHARED_EXPORT AnalysisPattern : public QObject, public Identity
{
public:
    AnalysisPattern();
    AnalysisPattern(const QString& name, const QString& description=sUNDEF);

    virtual bool execute(const QVariantMap& inputParameters, QVariantMap& outputParameters)  = 0;
    virtual IWorkflow workflow(const IOOptions& opt) = 0;

    virtual void store(QDataStream& stream);
    virtual void load(QDataStream& stream);
    virtual QString type() const = 0 ;

    void attachedModel(const IModel& model);
    IModel attachedModel();

private:
    IModel _attachedModel;
};

typedef std::shared_ptr<AnalysisPattern> SPAnalysisPattern;
}

#endif // ANALYSISPATTERN_H
