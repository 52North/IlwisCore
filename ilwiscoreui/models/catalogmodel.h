#ifndef CATALOGMODEL_H
#define CATALOGMODEL_H

#include <QObject>
#include <QFileInfo>
#include <QQmlListProperty>
#include "ilwisobjectmodel.h"
#include "resourcemodel.h"
#include "catalogview.h"
#include "catalogmapitem.h"
#include "ilwiscoreui_global.h"


//namespace Desktopclient {
class LayerManager;

class ILWISCOREUISHARED_EXPORT CatalogModel : public ResourceModel
{
    Q_OBJECT
public:
    Q_PROPERTY(QQmlListProperty<ResourceModel> resources READ resources NOTIFY contentChanged)
    Q_PROPERTY(QQmlListProperty<CatalogMapItem> mapItems READ mapItems NOTIFY mapItemsChanged)
    Q_PROPERTY(bool initNode READ initNode CONSTANT)
    Q_PROPERTY(int level READ level CONSTANT)
    Q_PROPERTY(int isScanned READ isScanned CONSTANT)
    Q_PROPERTY(bool canBeAnimated READ canBeAnimated CONSTANT)
    Q_PROPERTY(QString nameFilter READ nameFilter WRITE nameFilter NOTIFY contentChanged)

    ~CatalogModel();
    explicit CatalogModel(QObject *parent = 0);
    CatalogModel(const Ilwis::Resource& res, QObject *parent);
    CatalogModel(quint64 id, QObject *parent);

    Q_INVOKABLE void makeParent(QObject *obj);
    Q_INVOKABLE void filterChanged(const QString &typeIndication, bool state);
    Q_INVOKABLE void filter(const QString& filterString);
    Q_INVOKABLE void prepareMapItems(LayerManager *manager, bool force=false);
    Q_INVOKABLE QStringList objectCounts();

    bool isScanned() const;
    bool initNode() const;
    int level() const;
    QQmlListProperty<ResourceModel> resources();
    QQmlListProperty<CatalogMapItem> mapItems();

    void refresh(bool yesno);
    virtual void nameFilter(const QString&);
    bool canBeAnimated() const;
    QString nameFilter() const;

    void setView(const Ilwis::CatalogView &view, bool threading = false);
    Ilwis::CatalogView view() const;

protected:
    Ilwis::CatalogView _view;
    virtual void gatherItems();
   QList<ResourceModel *> _currentItems;
   QList<CatalogMapItem *> _catalogMapItems;
   std::map<quint64, int> _objectCounts;
   bool _refresh = true;


private:
    bool _isScanned;
    bool _initNode;
    int _level;
    std::map<QString, bool> _filterState;
    QString _nameFilter;

private slots:
    void updateContainer();
signals:
    void selectionChanged();
    void contentChanged();
    void mapItemsChanged();


};

template <typename T>
struct CatLess // public std::binary_function<bool, const T*, const T*>
{
  bool operator()(QObject* a, QObject* b) const
  {
      if ( a == nullptr || b == nullptr)
          return false;

      return static_cast<T *>(a)->displayName().toLower() < static_cast<T *>(b)->displayName().toLower();
  }
};

class CatalogWorker2 : public QObject {
    Q_OBJECT

public:
    CatalogWorker2(const QUrl& url);

public slots:
    void process();


signals:
    void finished();
    void updateContainer();


private:
    QUrl _container;
};
//}
//}

#endif // CATALOGMODEL_H
