#ifndef APPLICATINMODEL_H
#define APPLICATINMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT ApplicationModel : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationModel(QObject *parent = 0);

signals:

public slots:
};

#endif // APPLICATINMODEL_H
