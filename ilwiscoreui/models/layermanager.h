#ifndef VISUALIZATIONMANAGER_H
#define VISUALIZATIONMANAGER_H

#include <memory>
#include <QObject>

#include "coveragelayermodel.h"
#include "drawers/drawerinterface.h"
#include "drawers/layersviewcommandinterface.h"

namespace Ilwis {
    class GeoReference;
    typedef IlwisData<GeoReference> IGeoReference;
}

class CoverageLayerModel;
class UIContextModel;
class LayerInfoItem;

class ILWISCOREUISHARED_EXPORT LayerManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<CoverageLayerModel> layers READ layers NOTIFY layerChanged)
    Q_PROPERTY(QQmlListProperty<LayerInfoItem> layerInfoItems READ layerInfoItems NOTIFY layerInfoItemsChanged)
    Q_PROPERTY(bool zoomInMode READ zoomInMode WRITE setZoomInMode NOTIFY zoomInModeChanged)
    Q_PROPERTY(bool hasSelectionDrawer READ hasSelectionDrawer WRITE setHasSelectionDrawer NOTIFY hasSelectionDrawerChanged)
    Q_PROPERTY(ResourceModel * coordinateSystem READ coordinateSystem NOTIFY coordinateSystemChanged)
    Q_PROPERTY(QVariantMap viewEnvelope READ viewEnvelope NOTIFY viewEnvelopeChanged)
    Q_PROPERTY(QVariantMap latlonEnvelope READ latlonEnvelope NOTIFY latlonEnvelopeChanged)

public:
    enum LayerMovement{lmUP, lmDOWN, lmREMOVE};

    explicit LayerManager(QObject *parent = 0);
    LayerManager(QObject *parent, UIContextModel *context);

    Q_INVOKABLE void setLayerListName(const QString name);
    Q_INVOKABLE CoverageLayerModel* layer(quint32 layerIndex);
    Q_INVOKABLE void refresh();

    void addVisualizationModel(CoverageLayerModel* model);
    void addDataSource(const QUrl& url, IlwisTypes tp, Ilwis::Geodrawer::DrawerInterface *drawer);
    bool zoomInMode() const;
    void setZoomInMode(bool yesno) ;
    bool hasSelectionDrawer() const;
    void setHasSelectionDrawer(bool yesno);
    void setScreenGeoReference(const Ilwis::IGeoReference& grf);
    void moveLayer(int index, LayerMovement type);
    Ilwis::IGeoReference screenGrf() const;
    QString currentCoordinate() const;
    void setCurrentCoordinate(const QString& var);
    QString currentLatLon() const;
    QString layerListName() const;
    ResourceModel *coordinateSystem() const;
    Ilwis::Geodrawer::DrawerInterface *rootDrawer();
    const Ilwis::Geodrawer::DrawerInterface *rootDrawer() const;

    void layersView(LayersViewCommandInterface* view);
    QQmlListProperty<CoverageLayerModel> layers();
    QString layerInfo(const Ilwis::Coordinate& crd, const QString &attrName="");

signals:
    void removeLayer(const Ilwis::Resource& resource);
    void layerChanged();
    void zoomInModeChanged();
    void hasSelectionDrawerChanged();
    void layerInfoItemsChanged();
    void coordinateSystemChanged();
    void viewEnvelopeChanged();
    void latlonEnvelopeChanged();

public slots:

private:
    QQmlListProperty<LayerInfoItem> layerInfoItems();
    QList<LayerInfoItem *> _layerInfoItems;
    QList<CoverageLayerModel *> _layers;
    UIContextModel *_uicontext;
    Ilwis::IGeoReference _screenGrf;
    bool _zoomInMode = false;
    bool _hasSelectionDrawer = false;
    QString _layerListName;
    LayersViewCommandInterface *_layersView = 0;
    ResourceModel *_masterCsy = 0;
    void init();
    Ilwis::Envelope _viewEnvelope;
    QVariantMap viewEnvelope() const;
    QVariantMap latlonEnvelope() const;

};



#endif // VISUALIZATIONMANAGER_H
