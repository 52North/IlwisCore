#include <QString>
#include <QUrlQuery>

#include "identity.h"
#include "resource.h"
#include "wfsfeature.h"

using namespace Ilwis;
using namespace Wfs;

WfsFeature::WfsFeature()
{
}

WfsFeature::WfsFeature(const QUrl &rawfeatureUrl, const QUrl &normalizedUrl): Resource(normalizedUrl, rawfeatureUrl, itFEATURE)
{
    if ( !normalizedUrl.isValid()){
        if ( rawfeatureUrl.hasQuery()) {
            QString normUrl = rawfeatureUrl.toString(QUrl::RemoveQuery);
            QUrlQuery query(rawfeatureUrl);
            if ( !query.hasQueryItem("typeName")){
                setUrl(normUrl);
            } else {
                QString tpName = query.queryItemValue("typeName");
                setUrl(normUrl + "/" + tpName);
            }
        }
    }
}

WfsFeature::~WfsFeature()
{
}

QUrl WfsFeature::getFeatureUrl()
{
    return url();
}

QString WfsFeature::title() const
{
    return _title;
}

void WfsFeature::setTitle(QString title)
{
    _title = title;
}

Envelope WfsFeature::bbox() const
{
    return _bbox;
}

void WfsFeature::setBBox(Envelope &bbox)
{
    _bbox = bbox;
}
