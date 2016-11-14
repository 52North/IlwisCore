#ifndef MODELLERMODEL_H
#define MODELLERMODEL_H

#include <QObject>

#include "kernel_global.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "operationmetadata.h"
#include "ilwisobjectmodel.h"
#include "symboltable.h"
#include "workflow.h"
#include "applicationsetup.h"
#include "analysispattern.h"
#include "operationExpression.h"
#include "nodepropobject.h"
#include "edgepropobject.h"
#include "workflowmodel.h"
#include "conceptmodel.h"
#include "analysismodel.h"
#include "applicationmodel.h"
#include "ilwiscoreui_global.h"
#include "model.h"

class ILWISCOREUISHARED_EXPORT ModelDesigner : public ResourceModel
{
    Q_OBJECT
public:
    explicit ModelDesigner(QObject *parent = 0);
    ModelDesigner(ResourceModel *rmodel, QObject *parent=0);
    Q_PROPERTY(int workflowCount READ workflowCount NOTIFY workflowCountChanged)
    Q_PROPERTY(int analysisCount READ analysisCount NOTIFY analysisCountChanged)
    Q_PROPERTY(int conceptCount READ conceptCount NOTIFY conceptCountChanged)
    Q_PROPERTY(int applicationCount READ applicationCount NOTIFY applicationCountChanged)
    Q_PROPERTY(QStringList workflowNames READ workflowNames CONSTANT)
    Q_PROPERTY(QStringList analysisNames READ analysisNames NOTIFY analysisNamesChanged)
    Q_PROPERTY(QStringList applicationNames READ applicationNames CONSTANT)
    Q_PROPERTY(WorkflowModel *currentWorkflow READ currentWorkflow WRITE currentWorkflow NOTIFY workflowChanged)
    Q_PROPERTY(QStringList analysisTypes READ analysisTypes NOTIFY analysisTypesChanged)


    qint32 workflowCount() const;
    Q_INVOKABLE WorkflowModel* workflow(qint32 index) const;
    Q_INVOKABLE WorkflowModel* workflow(const QString& name) const;
    Q_INVOKABLE WorkflowModel *addWorkflow(const QString &filter);
    //Q_INVOKABLE bool addWorklfow(const IWorkflow& wf);
    Q_INVOKABLE void removeWorkflow(const QString& name);
    Q_INVOKABLE void removeWorkflow(qint32 index);

    QStringList analysisTypes() const;
    qint32 analysisCount() const;
    Q_INVOKABLE AnalysisModel *analysisPattern(qint32 index) const;
    Q_INVOKABLE bool addAnalysisPattern(AnalysisModel *amodel);
    Q_INVOKABLE void removeAnalysisPattern(const QString& name);
    Q_INVOKABLE void removeAnalysisPattern(qint32 index);

    qint32 applicationCount() const;
   // SPModelApplication application(qint32 index) const;
   // SPModelApplication application(const QString& name) const;
   // bool addApplication(ModelApplication *app);
    Q_INVOKABLE  void removeApplication(const QString& name);
    Q_INVOKABLE void removeApplication(qint32 index);

    Q_INVOKABLE bool store(const QString &location);

   qint32 conceptCount() const;



signals:
    void workflowCountChanged();
    void applicationCountChanged();
    void analysisCountChanged();
    void conceptCountChanged();
    void workflowChanged();
    void analysisTypesChanged();
    void analysisNamesChanged();

public slots:

private:
    std::vector<WorkflowModel*> _workflowmodels;
    std::vector<ApplicationModel*> _appmodels;
    std::vector<AnalysisModel*> _analysismodels;
    std::vector<ConceptModel *> _conceptmodels;
    WorkflowModel *_currentWorkflow = 0;
    IModel _model;

    QStringList workflowNames() const;
    QStringList applicationNames() const;
    QStringList analysisNames() const;
    QStringList conceptNames() const;
    WorkflowModel *currentWorkflow();
    void currentWorkflow(WorkflowModel *wf);
};

#endif // MODELLERMODEL_H
