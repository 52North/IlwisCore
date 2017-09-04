#ifndef WFS_H
#define WFS_H

#include <QUrl>
#include <QUrlQuery>

#include "wfsconnector_global.h"
#include "wfsresponse.h"

namespace Ilwis {
namespace  Wfs {

class WFSCONNECTORSHARED_EXPORT WebFeatureService
{

public:
    WebFeatureService(QUrl wfsUrl);

    QUrl url() const;
    SPWfsResponse getCapabilities(bool async=false) const;
    SPWfsResponse describeFeatureType(QUrlQuery query, bool async=false) const;
    SPWfsResponse getFeature(QUrlQuery query, bool async=false) const;

private:
    QUrl _resource;

    SPWfsResponse performRequest(QUrlQuery query, QString wfsRequest, bool async=false) const;
    SPWfsResponse performSyncRequest(QUrl request) const;
    SPWfsResponse performAsyncRequest(QUrl request) const;


};

}
}

#endif // WFS_H
