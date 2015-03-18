#ifndef VISUALIZATIONMANAGER_H
#define VISUALIZATIONMANAGER_H

#include <memory>
#include <QObject>

#include "coveragelayermodel.h"
#include "drawers/drawerinterface.h"
#include "drawers/layersviewcommandinterface.h"

class CoverageLayerModel;
class UIContextModel;

class ILWISCOREUISHARED_EXPORT LayerManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<CoverageLayerModel> layers READ layers NOTIFY layerChanged)
    Q_PROPERTY(bool zoomInMode READ zoomInMode WRITE setZoomInMode NOTIFY zoomInModeChanged)
    Q_PROPERTY(bool hasSelectionDrawer READ hasSelectionDrawer WRITE setHasSelectionDrawer NOTIFY hasSelectionDrawerChanged)

public:
    explicit LayerManager(QObject *parent = 0);
    LayerManager(QObject *parent, UIContextModel *context);

    void addVisualizationModel(CoverageLayerModel* model);
    void addDataSource(const QUrl& url, IlwisTypes tp, Ilwis::Geodrawer::DrawerInterface *drawer);
    bool zoomInMode() const;
    void setZoomInMode(bool yesno) ;
    bool hasSelectionDrawer() const;
    void setHasSelectionDrawer(bool yesno);
    QString currentCoordinate() const;
    void setCurrentCoordinate(const QString& var);
    QString currentLatLon() const;
    void layersView(LayersViewCommandInterface* view);
    QQmlListProperty<CoverageLayerModel> layers();
    Q_INVOKABLE CoverageLayerModel* layer(quint32 layerIndex);

signals:
    void removeLayer(const Ilwis::Resource& resource);
    void layerChanged();
    void zoomInModeChanged();
    void hasSelectionDrawerChanged();

public slots:

private:

    QList<CoverageLayerModel *> _layers;
    UIContextModel *_uicontext;
    bool _zoomInMode = false;
    bool _hasSelectionDrawer = false;
    LayersViewCommandInterface *_layersView = 0;
    void init();

};



#endif // VISUALIZATIONMANAGER_H
