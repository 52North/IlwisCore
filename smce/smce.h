#ifndef SMCE_H
#define SMCE_H

#include <QObject>

#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "node.h"
#include "smcemodel.h"

namespace Ilwis{
namespace Smce {

class SMCE : public AnalysisPattern
{

public:

    explicit SMCE();
    SMCE(const QString& name, const QString& description);
    bool execute(const QVariantMap& inputParameters, QVariantMap& outputParameters);
    Ilwis::IWorkflow workflow(const Ilwis::IOOptions& opt);
    virtual void store(QDataStream& stream);
    virtual void loadMetadata(QDataStream &stream);
    QString type() const;
    Node * root() const;
    static AnalysisPattern *create(const QString& name, const QString& description, const Ilwis::IOOptions &options);

signals:

public slots:

private:
    void Init();
    Node * _tree;

    NEW_ANALYSISPATTERN
};
}
}

#endif // SMCE_H
