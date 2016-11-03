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

class ILWISCOREUISHARED_EXPORT ModelDesigner : public QObject
{
    Q_OBJECT
public:
    explicit ModelDesigner(QObject *parent = 0);
    ModelDesigner(IModel& model, QObject *parent=0);
    Q_PROPERTY(int workflowCount READ workflowCount NOTIFY workflowCountChanged)
    Q_PROPERTY(int analysisCount READ analysisCount NOTIFY analysisCountChanged)
    Q_PROPERTY(int conceptCount READ conceptCount NOTIFY conceptCountChanged)
    Q_PROPERTY(int applicationCount READ applicationCount NOTIFY applicationCountChanged)
    Q_PROPERTY(QStringList workflowNames READ workflowNames CONSTANT)
    Q_PROPERTY(QStringList analysisNames READ analysisNames CONSTANT)
    Q_PROPERTY(QStringList applicationNames READ applicationNames CONSTANT)


    qint32 workflowCount() const;
    Q_INVOKABLE WorkflowModel* workflow(qint32 index) const;
    Q_INVOKABLE WorkflowModel* workflow(const QString& name) const;
    Q_INVOKABLE WorkflowModel *addWorkflow(const QString &filter);
    //Q_INVOKABLE bool addWorklfow(const IWorkflow& wf);
    Q_INVOKABLE void removeWorkflow(const QString& name);
    Q_INVOKABLE void removeWorkflow(qint32 index);


    qint32 analysisCount() const;
    //SPAnalysisPattern analysisPattern(qint32 index) const;
    //SPAnalysisPattern analysisPattern(const QString& name) const;
    //Q_INVOKABLE bool addAnalysisPattern(AnalysisPattern *pattern);
    Q_INVOKABLE void removeAnalysisPattern(const QString& name);
    Q_INVOKABLE void removeAnalysisPattern(qint32 index);

    qint32 applicationCount() const;
   // SPModelApplication application(qint32 index) const;
   // SPModelApplication application(const QString& name) const;
   // bool addApplication(ModelApplication *app);
    Q_INVOKABLE  void removeApplication(const QString& name);
    Q_INVOKABLE void removeApplication(qint32 index);

   qint32 conceptCount() const;



signals:
    void workflowCountChanged();
    void applicationCountChanged();
    void analysisCountChanged();
    void conceptCountChanged();

public slots:

private:
    std::vector<WorkflowModel*> _workflowmodels;
    std::vector<ApplicationModel*> _appmodels;
    std::vector<AnalysisModel*> _analysismodels;
    std::vector<ConceptModel *> _conceptmodels;
    IModel _model;

    QStringList workflowNames() const;
    QStringList applicationNames() const;
    QStringList analysisNames() const;
    QStringList conceptNames() const;
};

#endif // MODELLERMODEL_H
