#ifndef WORKFLOWCATALOGMODEL_H
#define WORKFLOWCATALOGMODEL_H

#include <QObject>
#include <QQmlListProperty>

#include "ilwiscoreui_global.h"

class ModelDesigner;

class ILWISCOREUISHARED_EXPORT ScenarioBuilderModel : public QObject
{
    Q_OBJECT
public:
    explicit ScenarioBuilderModel(QObject *parent = 0);

    Q_INVOKABLE ModelDesigner * create();

private:


signals:


public slots:

};


#endif // WORKFLOWCATALOGMODEL_H
