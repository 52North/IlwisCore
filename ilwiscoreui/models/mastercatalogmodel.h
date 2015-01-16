#ifndef CATALOGSMODEL_H
#define CATALOGSMODEL_H

#include <QQmlContext>
#include <QQmlListProperty>
#include <QQuickItem>
#include <deque>
#include "resourcemodel.h"
#include "catalogmodel.h"
#include "catalogview.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
class CatalogView;
}
//namespace Desktopclient{

typedef QQmlListProperty<ResourceModel> QMLResourceList;

class ILWISCOREUISHARED_EXPORT MasterCatalogModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<CatalogModel> bookmarked READ bookmarked CONSTANT)

    Q_PROPERTY(QMLResourceList leftResources READ leftResources NOTIFY leftResourcesChanged)
    Q_PROPERTY(QMLResourceList rightResources READ rightResources NOTIFY rightResourcesChanged)
    Q_PROPERTY(QQmlListProperty<IlwisObjectModel> selectedData READ selectedData NOTIFY selectionChanged)
    Q_PROPERTY(quint32 selectedBookmark READ selectedBookmark WRITE setSelectedBookmark)
    Q_PROPERTY(QString currentUrl READ currentUrl)
    Q_PROPERTY(int activeSplit READ activeSplit WRITE setActiveSplit NOTIFY activeSplitChanged)


public:
    MasterCatalogModel();
    MasterCatalogModel(QQmlContext *qmlcontext);
    QQmlListProperty<CatalogModel> bookmarked();
    quint32 selectedIndex() const;
    quint32 selectedBookmark() const;
    void setSelectedIndex(const QString& path);
    void setSelectedBookmark(quint32 index);
    QMLResourceList leftResources();
    QMLResourceList rightResources();
    void root(QObject *obj);
    QString currentUrl() const;
    int activeSplit() const;
    void setActiveSplit(int index);    

    Q_INVOKABLE  void addCatalog(const QString& path,int splitIndex);
    Q_INVOKABLE QString changeCatalog(const QString &inpath);
    Q_INVOKABLE QStringList driveList() const;
    Q_INVOKABLE QString getDrive(quint32 index);
    Q_INVOKABLE void addBookmark(const QString &path);
    Q_INVOKABLE void deleteBookmark(quint32 index);
    Q_INVOKABLE void setCatalogMetadata(const QString &displayName, const QString &description);
    Q_INVOKABLE void setObjectFilterCurrentCatalog(const QString& objecttype, bool state);
    Q_INVOKABLE ResourceModel *id2Resource(const QString& objectid);
   // Q_INVOKABLE IlwisObjectModel *id2object(const QString& objectid);
    Q_INVOKABLE QStringList knownCatalogs(bool fileonly=true);
    Q_INVOKABLE void setWorkingCatalog(const QString& path);
    Q_INVOKABLE void refreshWorkingCatalog();
    Q_INVOKABLE CatalogModel *selectedCatalog();
    CatalogModel *selectedCatalog() const;
    std::vector<Ilwis::Resource> select(const QString& filter);

    QQmlListProperty<IlwisObjectModel> selectedData();

public slots:
    void updateCatalog(const QUrl &url);
    
private:
    QList<CatalogModel *> _bookmarks;
    std::vector<std::deque<CatalogModel *>> _splitCatalogs;
    void setIndex(const QModelIndex &index, const QVariant &value, int role);
    QQmlContext *_qmlcontext = 0;
    QObject *_root = 0;
    QMLResourceList _currentList;
    //std::vector<QString> _currentUrl;
    int _activeSplit = 0;

     void pushToFront(int index);
    void emitResourcesChanged();
    
signals:
    void leftResourcesChanged();
    void rightResourcesChanged();
    void selectionChanged();
    void activeSplitChanged();
};
//}
//}

Q_DECLARE_METATYPE(QMLResourceList)

#endif // CATALOGSMODEL_H
