#include <QSqlQuery>
#include <QSqlError>
#include <fstream>
#include <iterator>

#include "geos/geom/LinearRing.h"

#include "kernel.h"
#include "module.h"
#include "coverage.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "columndefinition.h"
#include "table.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "textdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "ilwisobjectconnector.h"
#include "wfsparsingcontext.h"
#include "wfsfeatureconnector.h"
#include "wfsfeatureparser.h"
#include "wfsfeaturedescriptionparser.h"
#include "wfs.h"
#include "wfsfeature.h"
#include "wfsresponse.h"
#include "wfsutils.h"

using namespace Ilwis;
using namespace Wfs;

ConnectorInterface* WfsFeatureConnector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new WfsFeatureConnector(resource, load, options);
}

WfsFeatureConnector::WfsFeatureConnector(const Resource &resource, bool load, const Ilwis::IOOptions &options) :
    IlwisObjectConnector(resource,load, options) {
}


Ilwis::IlwisObject* WfsFeatureConnector::create() const {
    qDebug() << "WfsFeatureConnector::create -> FeatureCoverage";
    return new FeatureCoverage(this->_resource);
}

bool WfsFeatureConnector::loadMetaData(Ilwis::IlwisObject *data, const IOOptions& options)
{
    qDebug() << "WfsFeatureConnector::loadMetaData()";
    if (options.size() > 0) {
        qWarning() << "IOOptions not empty .. not handled for now.";
    }

    QUrl featureUrl = sourceRef().url(true);
    WebFeatureService wfs(featureUrl);
    QUrlQuery queryFeatureType(featureUrl);
    SPWfsResponse featureDescriptionResponse = wfs.describeFeatureType(queryFeatureType);
    WfsFeatureDescriptionParser schemaParser(featureDescriptionResponse);
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);

    _context.setResource(_resource);
    return schemaParser.parseMetadata(fcoverage, _context);
}

bool WfsFeatureConnector::loadData(IlwisObject *data, const IOOptions &options)
{
    qDebug() << "WfsFeatureConnector::loadData()";

    IOOptions iooptions = options.isEmpty() ? ioOptions() : options;
    //if(!loadMetaData(data, iooptions))
    //    return false;

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);

    QUrl featureUrl = sourceRef().url(true);
    WebFeatureService wfs(featureUrl);

    QUrlQuery queryFeature(featureUrl);
    SPWfsResponse response = wfs.getFeature(queryFeature);
    WfsFeatureParser featureParser(response, fcoverage);

    featureParser.context(_context);
    featureParser.parseFeatureMembers();

    _binaryIsLoaded = true;
    return _binaryIsLoaded;
}

IlwisTypes WfsFeatureConnector::ilwisType(const QString &resourceUrl)
{
    QUrl url(resourceUrl);
    if (WfsUtils::isValidWfsUrl(url)) {
        return itUNKNOWN;
    }

    QUrlQuery query(url);
    WfsUtils::lowerCaseKeys(query);
    QString request = query.queryItemValue("request");

    // TODO when stand-alone table connector is present!
    //if (request == "DescribeFeature") {
    //    return itTABLE;
    //}

    if (request == "GetFeature" || request == "DescribeFeature") {
        return itFEATURE;
    }

    return itUNKNOWN;
}


QString WfsFeatureConnector::provider() const
{
    return QString("wfs");
}
