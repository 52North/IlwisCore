#ifndef SMCEMODEL_H
#define SMCEMODEL_H

#include <QObject>
#include "node.h"
#include "workflow/analysismodel.h"

class SMCEModel : public AnalysisModel
{
    Q_OBJECT

    //Q_PROPERTY(Node tree READ tree)
    //Q_PROPERTY(QQmlListProperty<AnalysisModel> tree READ tree NOTIFY treeChanged)

public:
    SMCEModel();
    SMCEModel(Ilwis::AnalysisPattern *p);
    static AnalysisModel *create(Ilwis::AnalysisPattern *pattern);
    Q_INVOKABLE bool execute(const QVariantMap& inputParameters, QVariantMap& outputParameters);
    Q_INVOKABLE Node* tree() const;

private:

    NEW_ANALYSISMODEL

};

#endif // SMCEMODEL_H
