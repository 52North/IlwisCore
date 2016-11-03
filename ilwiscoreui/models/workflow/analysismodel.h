#ifndef ANALYSISMODEL_H
#define ANALYSISMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "workflow.h"
#include "analysispattern.h"

namespace Ilwis {
    class AnalysisPattern;
}

class ILWISCOREUISHARED_EXPORT AnalysisModel : public QObject
{
    Q_OBJECT
public:
    explicit AnalysisModel(QObject *parent = 0);

    QString name() const;

signals:

public slots:

private:
    std::unique_ptr<Ilwis::AnalysisPattern> _analysis;

};

#endif // ANALYSISMODEL_H
