#ifndef CATALOGMODEL_H
#define CATALOGMODEL_H

#include <QObject>
#include <QFileInfo>
#include <QQmlListProperty>
#include "ilwisobjectmodel.h"
#include "resourcemodel.h"
#include "catalogview.h"

namespace Ilwis {

}
//namespace Desktopclient {


class CatalogModel : public ResourceModel
{
    Q_OBJECT
public:
    Q_PROPERTY(QQmlListProperty<ResourceModel> resources READ resources CONSTANT)
    Q_PROPERTY(QQmlListProperty<IlwisObjectModel> selectedData READ selectedData NOTIFY selectionChanged)
    Q_PROPERTY(bool initNode READ initNode CONSTANT)
    Q_PROPERTY(int level READ level CONSTANT)
    Q_PROPERTY(int isScanned READ isScanned CONSTANT)

    CatalogModel() ;
    explicit CatalogModel(const Ilwis::CatalogView &view, int lvl, QObject *parent = 0);
    bool isScanned() const;
    bool initNode() const;
    int level() const;
    QQmlListProperty<ResourceModel> resources();
    QQmlListProperty<IlwisObjectModel> selectedData();
    void filterChanged(const QString &objectType, bool state);
    void refresh(bool yesno);
    Q_INVOKABLE void setSelectedObjects(const QString& objects);
protected:
    Ilwis::CatalogView _view;
    void newview(const Ilwis::CatalogView &view);
    void gatherItems();
   QList<ResourceModel *> _currentItems;
   QList<IlwisObjectModel *> _selectedObjects;

private:
    //bool _hasChilderen;
    bool _isScanned;
    bool _initNode;
    int _level;
    std::map<QString, bool> _filterState;

    bool _refresh = false;

signals:
    void selectionChanged();


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
