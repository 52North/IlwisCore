#ifndef CATALOGSMODEL_H
#define CATALOGSMODEL_H

#include <QQmlContext>
#include <QQmlListProperty>
#include <QQuickItem>
#include <deque>
#include "resourcemodel.h"
#include "catalogmodel.h"
#include "workspacemodel.h"
#include "catalogview.h"
#include "tranquilizer.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
class CatalogView;
}
//namespace Desktopclient{

class CatalogFilterModel;

typedef QQmlListProperty<ResourceModel> QMLResourceList;

class ILWISCOREUISHARED_EXPORT MasterCatalogModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<CatalogModel> bookmarked READ bookmarked NOTIFY bookmarksChanged)
    Q_PROPERTY(QQmlListProperty<CatalogFilterModel> defaultFilters READ defaultFilters CONSTANT)
    Q_PROPERTY(QQmlListProperty<WorkSpaceModel> workspaces READ workspaces NOTIFY workspacesChanged)
    Q_PROPERTY(QQmlListProperty<IlwisObjectModel> selectedData READ selectedData NOTIFY selectionChanged)
    Q_PROPERTY(int activeSplit READ activeSplit WRITE setActiveSplit NOTIFY activeSplitChanged)
    Q_PROPERTY(QString currentUrl READ currentUrl WRITE setCurrentUrl NOTIFY currentUrlChanged)
    Q_PROPERTY(CatalogModel* currentCatalog READ currentCatalog WRITE setCurrentCatalog NOTIFY currentCatalogChanged)



public:
    MasterCatalogModel();
    MasterCatalogModel(QQmlContext *qmlcontext);
    QQmlListProperty<CatalogModel> bookmarked();
    QQmlListProperty<WorkSpaceModel> workspaces();
    void setSelectedBookmark(quint32 index);
    int activeSplit() const;
    void setActiveSplit(int index);
    QString currentUrl() const;
    void setCurrentUrl(const QString& url);
    CatalogModel *currentCatalog() const;
    void setCurrentCatalog(CatalogModel * cat);
    WorkSpaceModel *workspace(quint64 id);
    quint32 workspaceIndex(const QString& name);

    Q_INVOKABLE quint32 selectedBookmark(const QString &url);
    Q_INVOKABLE  CatalogModel *newCatalog(const QString& inpath, const QString &filter="");
    Q_INVOKABLE QStringList driveList() const;
    Q_INVOKABLE QString getDrive(quint32 index);
    Q_INVOKABLE void addBookmark(const QString &path);
    Q_INVOKABLE void addWorkSpace(const QString& name);
    Q_INVOKABLE void removeWorkSpace(const QString& name);
    Q_INVOKABLE WorkSpaceModel *workspace(const QString& name);
    Q_INVOKABLE void deleteBookmark(quint32 index);
    Q_INVOKABLE void setCatalogMetadata(const QString &displayName, const QString &description);
    Q_INVOKABLE ResourceModel *id2Resource(const QString& objectid);
    Q_INVOKABLE QStringList knownCatalogs(bool fileonly=true);
    Q_INVOKABLE void setWorkingCatalog(const QString& path);
    Q_INVOKABLE void refreshWorkingCatalog();
    Q_INVOKABLE int activeTab() const;
    Q_INVOKABLE void setActiveTab(int value);
    Q_INVOKABLE QString getName(const QString& id);
    Q_INVOKABLE QString getUrl(const QString& id);
    Q_INVOKABLE QString id2type(const QString& id) const;
    Q_INVOKABLE IlwisObjectModel *id2object(const QString& objectid, QQuickItem *parent);
    Q_INVOKABLE QStringList select(const QString &filter, const QString& property);
    QQmlListProperty<IlwisObjectModel> selectedData();
    Q_INVOKABLE void setSelectedObjects(const QString& objects);
    Q_INVOKABLE bool hasSelectedObjects() const;
    Q_INVOKABLE QString selectedIds() const;
    Q_INVOKABLE void deleteObject(const QString& id);
    // for trq test
    Q_INVOKABLE void longAction();
    std::vector<Ilwis::Resource> select(const QString& filter);



public slots:
    void updateCatalog(const QUrl &url);
    void updateBookmarks() ;
    
private:
    QList<CatalogModel *> _bookmarks;
    QList<WorkSpaceModel *> _workspaces;
    QList<CatalogFilterModel *> _defaultFilters;
    QList<IlwisObjectModel *> _selectedObjects;
    QQmlContext *_qmlcontext = 0;
    QMLResourceList _currentList;
    int _selectedBookmarkIndex = 2; // from configuration
    QStringList _bookmarkids;
    int _activeSplit = 0;
    int _activeTab = 0;
    QString _currentUrl;
    CatalogModel *_currentCatalog = 0;

    
    CatalogModel *addBookmark(const QString &label, const QUrl &location, const QString &descr, const QString &query, bool threading = true);
    QString determineIdList(int dataCount, int operationCount, const QString &basekey);
    QList<std::pair<CatalogModel *, Ilwis::CatalogView> > startBackgroundScans(const std::vector<Ilwis::Resource>& catalogResources);
    void scanBookmarks();
    QQmlListProperty<CatalogFilterModel> defaultFilters();

    void loadWorkSpaces(const QString workspaceList);
    void setDefaultView();
    void addDefaultFilters();
signals:
    void selectionChanged();
    void activeSplitChanged();
    void currentUrlChanged();
    void currentCatalogChanged();
    void bookmarksChanged();
    void workspacesChanged();
};

class worker : public QObject{
    Q_OBJECT
public:
    worker() {

    }

    void process();

};

class CatalogWorker : public QObject {
    Q_OBJECT

public:
    CatalogWorker(QList<std::pair<CatalogModel *, Ilwis::CatalogView>>&models);
    ~CatalogWorker();

public slots:
    void process();


signals:
    void finished();
    void updateBookmarks();


private:
    QList<std::pair<CatalogModel *, Ilwis::CatalogView>> _models;
    void calculatelatLonEnvelopes();
    void calcLatLon(const Ilwis::ICoordinateSystem &csyWgs84, Ilwis::Resource &resource, std::vector<Ilwis::Resource> &updatedResources);
};
//}
//}

Q_DECLARE_METATYPE(QMLResourceList)

#endif // CATALOGSMODEL_H
