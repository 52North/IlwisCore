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
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "internalfeaturecoverageconnector.h"

using namespace Ilwis;
using namespace Internal;

InternalFeatureCoverageConnector::InternalFeatureCoverageConnector(const Ilwis::Resource &resource, bool load, const PrepareOptions &options) : IlwisObjectConnector(resource, load, options)
{

}

bool InternalFeatureCoverageConnector::loadMetaData(IlwisObject *data)
{
    return true;
}

bool InternalFeatureCoverageConnector::loadData(Ilwis::IlwisObject *)
{
    _binaryIsLoaded = true;
    return true;
}

QString InternalFeatureCoverageConnector::provider() const
{
    return "internal";
}

ConnectorInterface *InternalFeatureCoverageConnector::create(const Ilwis::Resource &resource, bool load, const PrepareOptions &options)
{
    return new  InternalFeatureCoverageConnector(resource, load, options);
}

IlwisObject *InternalFeatureCoverageConnector::create() const
{
    return new FeatureCoverage();
}
