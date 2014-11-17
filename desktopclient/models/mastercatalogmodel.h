#ifndef CATALOGSMODEL_H
#define CATALOGSMODEL_H

#include <QQmlContext>
#include <QQmlListProperty>
#include <QQuickItem>
#include "resourcemodel.h"
#include "catalogmodel.h"
#include "catalogview.h"

namespace Ilwis {
class CatalogView;
}
//namespace Desktopclient{

typedef QQmlListProperty<ResourceModel> QMLResourceList;

class MasterCatalogModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<CatalogModel> bookmarked READ bookmarked CONSTANT)

    Q_PROPERTY(QMLResourceList resources READ resources NOTIFY resourcesChanged)
    Q_PROPERTY(QQmlListProperty<IlwisObjectModel> selectedData READ selectedData NOTIFY selectionChanged)
    Q_PROPERTY(quint32 selectedBookmark READ selectedBookmark WRITE setSelectedBookmark NOTIFY resourcesChanged)


public:
    MasterCatalogModel();
    MasterCatalogModel(QQmlContext *qmlcontext);
    QQmlListProperty<CatalogModel> bookmarked();
    quint32 selectedIndex() const;
    quint32 selectedBookmark() const;
    void setSelectedIndex(const QString& path);
    void setSelectedBookmark(quint32 index);
    QMLResourceList resources();
    void root(QObject *obj);

    Q_INVOKABLE  void addCatalog(const QString& path);
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

    QQmlListProperty<IlwisObjectModel> selectedData();
public slots:
    void updateCatalog(const QUrl &url);

private:
    std::vector<QString> _bookmarkedList;
    QList<CatalogModel *> _bookmarks;
    std::map<QString, CatalogModel *> _activeCatalogs;
    void setIndex(const QModelIndex &index, const QVariant &value, int role);
    QQmlContext *_qmlcontext = 0;
    QObject *_root = 0;
    QMLResourceList _currentList;
    QString _currentUrl;
    
signals:
    void resourcesChanged();
    void selectionChanged();
};
//}
//}

Q_DECLARE_METATYPE(QMLResourceList)

#endif // CATALOGSMODEL_H
