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
    explicit ApplicationModel(QObject *parent = 0);

    QString name() const;

signals:

public slots:

private:
    Ilwis::ModelApplication *_app;
};

#endif // APPLICATINMODEL_H
