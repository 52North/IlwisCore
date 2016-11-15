#ifndef APPLICATINMODEL_H
#define APPLICATINMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

namespace Ilwis {
    class ModelApplication;
}
class ILWISCOREUISHARED_EXPORT ApplicationModel : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationModel(Ilwis::ModelApplication *app=0, QObject *parent = 0);
    Q_INVOKABLE QString panel(const QString& panelName);

    QString name() const;
    QString attachedAnalysis() const;
    void attachedAnalysis(const QString& name);

signals:

public slots:

protected:
    std::map<QString, QString> _panels;
    Ilwis::ModelApplication *_app = 0;

};

#endif // APPLICATINMODEL_H
