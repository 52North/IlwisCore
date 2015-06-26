#include "catalogmapitem.h"

CatalogMapItem::CatalogMapItem()
{

}

CatalogMapItem::CatalogMapItem(const std::vector<Ilwis::Resource> &res, const Ilwis::IGeoReference& grf, QObject *parent) : QObject(parent), _resources(res), _screenGrf(grf)
{

}

CatalogMapItem::~CatalogMapItem()
{

}

QString CatalogMapItem::latLonEnvelope() const
{
    return _resources[0]["latlonenvelope"].toString();
}

QString CatalogMapItem::catalogItemType()
{
    return Ilwis::IlwisObject::type2Name(_resources[0].ilwisType());
}

QString CatalogMapItem::name() const
{
    if ( _resources[0].isValid()){
        return _resources[0].name();
    }
    return "";
}

QVariantMap CatalogMapItem::drawEnvelope() const{
    QVariantMap vmap;
    try {
        if ( !_resources[0].isValid())
            return QVariantMap();

        QString env= _resources[0]["latlonenvelope"].toString();
        Ilwis::Envelope llenv(env);

        if ( llenv.isValid() && !llenv.isNull() && _screenGrf.isValid())    {

            Ilwis::BoundingBox bb = _screenGrf->coord2Pixel(llenv);
            vmap["minx"] = bb.min_corner().x;
            vmap["miny"] = bb.min_corner().y;
            vmap["width"] = bb.xlength();
            vmap["height"] = bb.ylength();
        }
        return vmap;
    } catch (const Ilwis::ErrorObject& err){

    } catch (std::exception& ex){
        Ilwis::kernel()->issues()->log(ex.what());
    }
    return vmap;
}

