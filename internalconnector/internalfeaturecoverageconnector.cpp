#include "kernel.h"
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "polygon.h"
#include "geometry.h"
#include "feature.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "ilwisobjectconnector.h"
#include "internalrastercoverageconnector.h"
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
