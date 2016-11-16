#ifndef APPLICATINMODEL_H
#define APPLICATINMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

#define NEW_APPMODEL \
    private: \
static ApplicationModelUI *dummy_app;

#define REGISTER_APPMODEL(classname,modelname) \
    ApplicationModelUI *classname::dummy_app = modelbuilder()->registerApplicationModel(modelname, classname::create);

namespace Ilwis {
    class ApplicationModel;
}
class ILWISCOREUISHARED_EXPORT ApplicationModelUI : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationModelUI(QObject *parent = 0);
    ApplicationModelUI(Ilwis::ApplicationModel *, QObject *parent = 0);
    Q_INVOKABLE QString panel(const QString& panelName);

    QString name() const;
    QString attachedAnalysis() const;
    void attachedAnalysis(const QString& name);

signals:

public slots:

protected:
    std::map<QString, QString> _panels;
    Ilwis::ApplicationModel *_app = 0;

};

#endif // APPLICATINMODEL_H
