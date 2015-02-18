#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include "visualizationmanager.h"
#include "coverage.h"
#include "geodrawer.h"
#include "georenderer.h"

QQuickFramebufferObject::Renderer *GeoDrawer::createRenderer() const
{
    return new GeoRenderer();
}

void GeoDrawer::setAttribute(const QString &drawercode, const QVariantMap &values)
{
    _attributeQueue.push_back(std::pair<QString, QVariantMap>(drawercode, values));

}

void GeoDrawer::removeDrawer(const QString &namecode, bool ascode)
{
    _removedDrawers.push_back({namecode, ascode});
    for(auto iter = _datasources.begin(); iter != _datasources.end(); ++iter){
        if ( ascode){
            if ( (*iter)._drawerCode == namecode){
                _datasources.erase(iter);
                break;
            }
        }else if ((*iter)._drawerName == namecode) {
            _datasources.erase(iter);
        }
    }
}

void GeoDrawer::addDataSource(const QString &url, const QString &typeName)
{
    try {
        if ( url == "" || typeName == "")
            return;


        IlwisTypes tp = Ilwis::IlwisObject::name2Type(typeName);
        if ( tp == itUNKNOWN)
            return;


        quint64 id = mastercatalog()->url2id(QUrl(url),tp);
        if ( id == i64UNDEF){
            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, TR("Visualization"), url);
            return ;
        }
        Resource resource = mastercatalog()->id2Resource(id);
        if (! resource.isValid()){
            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, TR("Visualization"), url);
            return ;
        }

        _datasources.push_back( DrawerIdTag(resource));

    } catch ( const ErrorObject& err){

    } catch ( const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
}


