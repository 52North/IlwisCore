#ifndef VISUALIZATIONMANAGER_H
#define VISUALIZATIONMANAGER_H

#include <memory>
#include <QObject>

#include "objectvisualizationmodel.h"

class ObjectVisualizationModel;


class VisualizationManager : public QObject
{
    Q_OBJECT

public:
    explicit VisualizationManager(QObject *parent = 0);

    void addVisualizationModel(ObjectVisualizationModel *newmodel);

    Q_INVOKABLE ObjectVisualizationModel *layer(quint32 layerIndex);

signals:
    void removeLayer(const Ilwis::Resource& resource);

public slots:

private:

    QList<ObjectVisualizationModel *> _layers;
    void init();

};



#endif // VISUALIZATIONMANAGER_H
