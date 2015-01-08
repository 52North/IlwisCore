#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "table.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "internalfeaturecoverageconnector.h"

using namespace Ilwis;
using namespace Internal;

InternalFeatureCoverageConnector::InternalFeatureCoverageConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{

}

bool InternalFeatureCoverageConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    return true;
}

bool InternalFeatureCoverageConnector::loadData(Ilwis::IlwisObject *, const IOOptions & )
{
    _binaryIsLoaded = true;
    return true;
}

QString InternalFeatureCoverageConnector::provider() const
{
    return "internal";
}

ConnectorInterface *InternalFeatureCoverageConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
    return new  InternalFeatureCoverageConnector(resource, load, options);
}

IlwisObject *InternalFeatureCoverageConnector::create() const
{
    return new FeatureCoverage();
}
