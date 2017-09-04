#ifndef GEOREFCONNECTOR_H
#define GEOREFCONNECTOR_H

namespace Ilwis {
namespace Gdal {

class GeorefConnector : public GdalConnector
{
public:
    GeorefConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    IlwisObject *create() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
};
}
}

#endif // GEOREFCONNECTOR_H
