#ifndef CATALOGVIEWMANAGER_H
#define CATALOGVIEWMANAGER_H

#include <QObject>
#include <QTimer>
#include <set>
#include <memory>
#include <mutex>
#include "kernel_global.h"

class CatalogModel;

namespace Ilwis {
typedef std::set<QUrl> UrlSet;

class CatalogViewManager : public QObject
{
    Q_OBJECT

    friend Ilwis::CatalogViewManager* Ilwis::catalogViewManager(QObject *parent);
public:


    void registerCatalogModel(CatalogModel *model);
    void unRegisterCatalogModel(CatalogModel *model);

signals:

public slots:
    void updateCatalogViews(const Ilwis::UrlSet& containers);
    void doUpdate();

private:
    explicit CatalogViewManager(QObject *parent = 0);
    std::map<CatalogModel *, bool> _catalogViews;

    static CatalogViewManager *_catalogViewManager;
    static QTimer *_timer;
    std::mutex _lock;
};

CatalogViewManager* catalogViewManager(QObject *parent=0);
}

#endif // CATALOGVIEWMANAGER_H
