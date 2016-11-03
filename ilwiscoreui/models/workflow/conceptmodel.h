#ifndef CONCEPMODEL_H
#define CONCEPMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT ConceptModel : public QObject
{
    Q_OBJECT
public:
    explicit ConceptModel(QObject *parent = 0);

signals:

public slots:
};

#endif // CONCEPMODEL_H
