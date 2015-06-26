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
    Q_PROPERTY(QQmlListProperty<IlwisObjectModel> selectedData READ selectedData NOTIFY selectionChanged)
    Q_PROPERTY(QQmlListProperty<CatalogMapItem> mapItems READ mapItems NOTIFY mapItemsChanged)
    Q_PROPERTY(bool initNode READ initNode CONSTANT)
    Q_PROPERTY(int level READ level CONSTANT)
    Q_PROPERTY(int isScanned READ isScanned CONSTANT)
    Q_PROPERTY(QString nameFilter READ nameFilter WRITE nameFilter NOTIFY contentChanged)

    ~CatalogModel();
    explicit CatalogModel(QObject *parent = 0);
    CatalogModel(const Ilwis::Resource& res, QObject *parent);
    bool isScanned() const;
    bool initNode() const;
    int level() const;
    QQmlListProperty<ResourceModel> resources();
    QQmlListProperty<IlwisObjectModel> selectedData();
    QQmlListProperty<CatalogMapItem> mapItems();
    Q_INVOKABLE void makeParent(QObject *obj);
    Q_INVOKABLE void filterChanged(const QString &objectType, bool state);
    Q_INVOKABLE void filter(const QString& filterString);
    void refresh(bool yesno);
    Q_INVOKABLE void setSelectedObjects(const QString& objects);
    virtual void nameFilter(const QString&);
    QString nameFilter() const;
    Q_INVOKABLE void prepareMapItems(LayerManager *manager, bool force=false);
    void setView(const Ilwis::CatalogView &view);
    Ilwis::CatalogView view() const;

    Q_INVOKABLE QString selectedIds() const;
protected:
    Ilwis::CatalogView _view;
    virtual void gatherItems();
   QList<ResourceModel *> _currentItems;
   QList<IlwisObjectModel *> _selectedObjects;
   QList<CatalogMapItem *> _catalogMapItems;
   bool _refresh = false;


private:
    //bool _hasChilderen;
    bool _isScanned;
    bool _initNode;
    int _level;
    std::map<QString, bool> _filterState;
    QString _nameFilter;


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
//}
//}

#endif // CATALOGMODEL_H
