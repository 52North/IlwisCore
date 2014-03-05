#include "kernel.h"
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "internalfeaturecoverageconnector.h"

using namespace Ilwis;
using namespace Internal;

InternalFeatureCoverageConnector::InternalFeatureCoverageConnector(const Ilwis::Resource &resource, bool load) : IlwisObjectConnector(resource, load)
{

}

bool InternalFeatureCoverageConnector::loadMetaData(IlwisObject *data)
{
    return true;
}

bool InternalFeatureCoverageConnector::loadBinaryData(Ilwis::IlwisObject *)
{
    _binaryIsLoaded = true;
    return true;
}

QString InternalFeatureCoverageConnector::provider() const
{
    return "internal";
}

ConnectorInterface *InternalFeatureCoverageConnector::create(const Ilwis::Resource &resource, bool load)
{
    return new  InternalFeatureCoverageConnector(resource, load);
}

IlwisObject *InternalFeatureCoverageConnector::create() const
{
    return new FeatureCoverage();
}
