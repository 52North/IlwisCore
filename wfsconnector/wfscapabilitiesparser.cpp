#include <sstream>

#include <QUrl>
#include <QString>
#include <QXmlQuery>
#include <QXmlStreamReader>
#include <QXmlResultItems>

#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "xpathparser.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "table.h"

#include "wfsresponse.h"
#include "wfsfeature.h"
#include "wfscapabilitiesparser.h"
#include "wfsutils.h"

using namespace Ilwis;
using namespace Wfs;

WfsCapabilitiesParser::WfsCapabilitiesParser(const SPWfsResponse response, const Resource wfsResource, const IOOptions &options): _wfsResource(wfsResource), _options(options)
{
    _parser = UPXPathParser(new XPathParser(response->device()));
    _parser->addNamespaceMapping("wfs", "http://www.opengis.net/wfs");
    _parser->addNamespaceMapping("ows", "http://www.opengis.net/ows");
}

void WfsCapabilitiesParser::parseFeatures(std::vector<Resource> &wfsFeatures)
{
    qDebug() << "WfsCapabilitiesParser::parseFeatures()";

    QXmlResultItems results;
    UPXmlQuery& query = _parser->queryFromRoot("//wfs:WFS_Capabilities/wfs:FeatureTypeList/wfs:FeatureType");

    if (query->isValid()) {
        query->evaluateTo( &results);
        QXmlItem item(results.next());
        while (!item.isNull()) {
            WfsFeature feature;
            parseFeature(item, feature);
            wfsFeatures.push_back(feature);
            item = results.next();
        }
        if (results.hasError()) {
            ERROR0(TR(QString("XPath evaluation failed.")));
        }
    }
}

void WfsCapabilitiesParser::parseFeature(QXmlItem &item, WfsFeature &feature)
{
    QUrl rawUrl, normalizedUrl;
    QString name = valueOf(item, "./wfs:Name/string()");
    createGetFeatureUrl(name, rawUrl, normalizedUrl);
    feature = WfsFeature(rawUrl, normalizedUrl);
    feature.name(name, false);
    feature.setTitle(valueOf(item, "./wfs:Title/string()"));
    feature.setDescription(valueOf(item, "./wfs:Abstract/string()"));

    QString code = valueOf(item, "./wfs:DefaultSRS/string()");
    QString srs = QString("code=").append(WfsUtils::normalizeEpsgCode(code));
    QString llText = valueOf(item, "./ows:WGS84BoundingBox/ows:LowerCorner/string()");
    QString urText = valueOf(item, "./ows:WGS84BoundingBox/ows:UpperCorner/string()");

    feature.addProperty("coordinatesystem", srs);
    feature.addProperty("envelope.ll", llText);
    feature.addProperty("envelope.ur", urText);
    feature.dimensions(urText + "," + llText);
    feature.addProperty("domain",name);

    if (_options.contains("forceXY")) {
        // override if forced XY axes order
        feature.addProperty("forceXY", _options["forceXY"].toBool());
    } else {
        feature.addProperty("forceXY", false);
    }
}

QString WfsCapabilitiesParser::valueOf(const QXmlItem &item, const QString &xpathQuery) {
    QString value;
    UPXmlQuery& query = _parser->queryRelativeFrom(item, xpathQuery);
    query->evaluateTo(&value);
    value = value.trimmed();
    return value;
}

void WfsCapabilitiesParser::createGetFeatureUrl(const QString& featureName, QUrl& rawUrl, QUrl& normalizedUrl) const
{
    QUrlQuery query;
    query.addQueryItem("service", "WFS");
    query.addQueryItem("version", "1.1.0");
    query.addQueryItem("request", "GetFeature");
    query.addQueryItem("typeName", featureName);
    rawUrl = _wfsResource.url(); // copy
    rawUrl.setQuery(query);
    normalizedUrl = _wfsResource.url().toString(QUrl::RemoveQuery) + "/" + featureName;
}

