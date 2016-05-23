#ifndef CATALOGEXPLORERWORKER_H
#define CATALOGEXPLORERWORKER_H

#include <QObject>
#include <memory>

#include "iooptions.h"

namespace Ilwis {

class Catalog;
class CatalogExplorer;
class IOOptions;

typedef std::shared_ptr<CatalogExplorer> SPCatalogExplorer;

}
class CatalogExplorerWorker : public QObject {
    Q_OBJECT

public:
    CatalogExplorerWorker(std::vector<std::pair<Ilwis::SPCatalogExplorer, Ilwis::IOOptions>> explorers) : _explorers(explorers){}

public slots:
    void process();


signals:
    void finished();
    void updateContainer();

private:
    std::vector<std::pair<Ilwis::SPCatalogExplorer, Ilwis::IOOptions>> _explorers;
};

#endif // CATALOGEXPLORERWORKER_H
