#ifndef WFSCAPABILITIESPARSER_H
#define WFSCAPABILITIESPARSER_H

#include "wfsconnector_global.h"

#include <QList>

#include "xpathparser.h"

class QUrl;
class QXmlItem;

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsCapabilitiesParser
{
public:
    WfsCapabilitiesParser(const SPWfsResponse response, const Resource wfsResource, const IOOptions &options = IOOptions());

    void parseFeatures(std::vector<Ilwis::Resource> &wfsFeaturesTypes);

private:
    Resource _wfsResource;
    IOOptions _options;
    UPXPathParser _parser;

    void createGetFeatureUrl(const QString &featureType, QUrl &rawUrl, QUrl &normalizedUrl) const;
    void parseFeature(QXmlItem &item, WfsFeature &feature);

    QString valueOf(const QXmlItem &item, const QString &xpathQuery);
};
}
}

#endif // WFSCAPABILITIESPARSER_H
