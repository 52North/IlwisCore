#include "kernel.h"
#include "catalogexplorer.h"

using namespace Ilwis;

CatalogExplorer::CatalogExplorer()
{
}

CatalogExplorer::CatalogExplorer(const Resource &resource, const IOOptions &options) : _resource(resource), _options(options)
{

}

CatalogExplorer::~CatalogExplorer()
{

}

Resource CatalogExplorer::source() const
{
    return _resource;
}

IOOptions CatalogExplorer::ioOptions() const
{
    return _options;
}

IOOptions &CatalogExplorer::ioOptionsRef()
{
    return _options;
}

QFileInfo CatalogExplorer::resolve2Local() const
{
    return _resource.toLocalFile();
}

CatalogExplorer::ExplorerType CatalogExplorer::explorerType() const
{
    return etEXPLICIT;
}

