#ifndef WORKFLOWCATALOGMODEL_H
#define WORKFLOWCATALOGMODEL_H

#include <QObject>
#include <QQmlListProperty>

#include "ilwiscoreui_global.h"

class ModelDesigner;

class ILWISCOREUISHARED_EXPORT ModelBuilder : public QObject
{
    Q_OBJECT
public:
    explicit ModelBuilder(QObject *parent = 0);

    Q_INVOKABLE ModelDesigner * createModel(QObject *parent);

private:


signals:


public slots:

};


#endif // WORKFLOWCATALOGMODEL_H
