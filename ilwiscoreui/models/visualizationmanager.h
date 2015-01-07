#ifndef VISUALIZATIONMANAGER_H
#define VISUALIZATIONMANAGER_H

#include <memory>
#include <QObject>

#include "objectvisualizationmodel.h"

class ObjectVisualizationModel;
class UIContextModel;

class ILWISCOREUISHARED_EXPORT VisualizationManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<ObjectVisualizationModel> layers READ layers NOTIFY layerChanged)

public:
    explicit VisualizationManager(QObject *parent = 0);
    VisualizationManager(QObject *parent, UIContextModel *context);

    void addVisualizationModel(ObjectVisualizationModel *newmodel);
    Q_INVOKABLE void addDataSource(const QString& url, const QString& typeName);
    QQmlListProperty<ObjectVisualizationModel> layers();
    Q_INVOKABLE ObjectVisualizationModel *layer(quint32 layerIndex);

signals:
    void removeLayer(const Ilwis::Resource& resource);
    void layerChanged();

public slots:

private:

    QList<ObjectVisualizationModel *> _layers;
    UIContextModel *_uicontext;
    void init();

};



#endif // VISUALIZATIONMANAGER_H
