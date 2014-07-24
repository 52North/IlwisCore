#include "kernel.h"
#include "catalogexplorer.h"

using namespace Ilwis;

CatalogExplorer::CatalogExplorer()
{
}

CatalogExplorer::CatalogExplorer(const Resource &resource, const IOOptions &) : _resource(resource)
{

}

CatalogExplorer::~CatalogExplorer()
{

}

Resource CatalogExplorer::source() const
{
    return _resource;
}

QFileInfo CatalogExplorer::resolve2Local() const
{
    return _resource.toLocalFile();
}
