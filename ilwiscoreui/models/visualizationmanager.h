#ifndef VISUALIZATIONMANAGER_H
#define VISUALIZATIONMANAGER_H

#include <memory>
#include <QObject>

#include "coveragelayermodel.h"
#include "drawers/drawerinterface.h"

class CoverageLayerModel;
class UIContextModel;

class ILWISCOREUISHARED_EXPORT VisualizationManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<CoverageLayerModel> layers READ layers NOTIFY layerChanged)

public:
    explicit VisualizationManager(QObject *parent = 0);
    VisualizationManager(QObject *parent, UIContextModel *context);

    void addVisualizationModel(CoverageLayerModel* model);
    void addDataSource(const QString& url, const QString& typeName, Ilwis::Geodrawer::DrawerInterface *drawer);
    QQmlListProperty<CoverageLayerModel> layers();
    Q_INVOKABLE CoverageLayerModel* layer(quint32 layerIndex);

signals:
    void removeLayer(const Ilwis::Resource& resource);
    void layerChanged();

public slots:

private:

    QList<CoverageLayerModel *> _layers;
    UIContextModel *_uicontext;
    void init();

};



#endif // VISUALIZATIONMANAGER_H
