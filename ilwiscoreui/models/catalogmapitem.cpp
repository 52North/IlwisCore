#include "catalogmapitem.h"

CatalogMapItem::CatalogMapItem()
{

}

CatalogMapItem::CatalogMapItem(const Ilwis::ICoverage &cov, const Ilwis::IGeoReference& grf, QObject *parent) : QObject(parent), _coverage(cov), _screenGrf(grf)
{

}

CatalogMapItem::~CatalogMapItem()
{

}

QString CatalogMapItem::latLonEnvelope() const
{
    return _coverage->envelope(true).toString();
}

QString CatalogMapItem::catalogItemType()
{
    return Ilwis::IlwisObject::type2Name(_coverage->ilwisType());
}

QString CatalogMapItem::name() const
{
    if ( _coverage.isValid()){
        return _coverage->name();
    }
    return "";
}

QVariantMap CatalogMapItem::drawEnvelope() const{
    QVariantMap vmap;
    try {
        if ( !_coverage.isValid())
            return QVariantMap();

        Ilwis::Envelope llenv = _coverage->envelope(true);

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

