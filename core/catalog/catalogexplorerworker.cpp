#include "kernel.h"
#include "catalog.h"
#include "catalogexplorer.h"
#include "catalogexplorerworker.h"

void CatalogExplorerWorker::process(){
    try{
        for(auto explorer : _explorers){
            std::vector<Ilwis::Resource> items = explorer.first->loadItems(explorer.second);
            Ilwis::mastercatalog()->addItems(items);
        }
        emit finished();
        emit updateContainer();

    } catch(const Ilwis::ErrorObject& ){

    } catch ( const std::exception& ex){
        Ilwis::kernel()->issues()->log(ex.what());
    }
}
