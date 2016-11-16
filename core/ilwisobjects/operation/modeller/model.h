#ifndef MODEL_H
#define MODEL_H

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT Model : public IlwisObject
{
public:
    Model();
    Model(const Resource &resource);

    IlwisTypes ilwisType() const;

    qint32 workflowCount() const;
    IWorkflow workflow(qint32 index) const;
    IWorkflow workflow(const QString& name) const;
    bool addWorklfow(const IWorkflow& wf);
    void removeWorkflow(const QString& name);
    void removeWorkflow(qint32 index);


    qint32 analysisCount() const;
    SPAnalysisPattern analysisPattern(qint32 index) const;
    SPAnalysisPattern analysisPattern(const QString& name) const;
    bool addAnalysisPattern(AnalysisPattern *pattern);
    void removeAnalysisPattern(const QString& name);
    void removeAnalysisPattern(qint32 index);

    qint32 applicationCount() const;
    SPModelApplication application(qint32 index) const;
    SPModelApplication application(const QString& name) const;
    bool addApplication(ApplicationModel *app);
    void removeApplication(const QString& name);
    void removeApplication(qint32 index);

private:
    std::vector<IWorkflow> _workflows;
    std::vector<SPAnalysisPattern> _analysisPatterns;
    std::vector<SPModelApplication> _applications;
};

typedef Ilwis::IlwisData<Ilwis::Model> IModel;
}



#endif // MODEL_H
