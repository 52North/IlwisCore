#ifndef ANALYSISMODEL_H
#define ANALYSISMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"


#define NEW_ANALYSISMODEL \
    private: \
static AnalysisModel *dummy_analysis;

#define REGISTER_ANALYSISMODEL(classname,modelname) \
    AnalysisModel *classname::dummy_analysis = modelbuilder()->registerAnalysisModel(modelname, classname::create);

namespace Ilwis {
    class AnalysisPattern;
}

class ILWISCOREUISHARED_EXPORT AnalysisModel : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString longName READ longName WRITE longName NOTIFY longNameChanged)
    Q_PROPERTY(QString mainPanel READ mainPanel CONSTANT)

    explicit AnalysisModel(Ilwis::AnalysisPattern *pattern = 0, QObject *parent=0);
    Q_INVOKABLE void suicide();


    QString name() const;

signals:
    void longNameChanged();
public slots:

protected:
    QString _panelPath;

private:
    Ilwis::AnalysisPattern *_analysis; // pattern is stored in the model; there is no ownership here


    QString longName() const;
    void longName(const QString& lname);
    Ilwis::Resource& resourceRef();
    const Ilwis::Resource &resourceRef() const;
    QString mainPanel() const;
};

#endif // ANALYSISMODEL_H
