#include "kernel.h"
#include "catalog.h"
#include "catalogexplorer.h"
#include "oshelper.h"
#include "catalogexplorerworker.h"

void CatalogExplorerWorker::process(){
    try{
        for(auto explorer : _explorers){
            std::vector<Ilwis::Resource> items = explorer.first->loadItems(explorer.second);
            Ilwis::mastercatalog()->addItems(items, true);
        }
        Ilwis::Resource location;
        if ( _explorers.size() > 0){
            location = _explorers[0].first->source();
        }

        if ( location.isValid() && Ilwis::Catalog::isSpatialDataLocation(location.url())){
            QString query = QString("(type & %1) != 0 and container='%2'").arg(QString::number(itCOVERAGE)).
                    arg(Ilwis::OSHelper::neutralizeFileName(location.url().toString()));
            calculatelatLonEnvelopes(query, location.name());

        }
        emit finished();
       return;

    } catch(const Ilwis::ErrorObject& ){

    } catch ( const std::exception& ex){
        Ilwis::kernel()->issues()->log(ex.what());
    }
    emit finished();
}
