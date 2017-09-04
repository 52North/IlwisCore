#ifndef COORDINATESYSTEMCONNECTOR_H
#define COORDINATESYSTEMCONNECTOR_H

namespace Ilwis {
namespace Ilwis3{
class CoordinateSystemConnector : public Ilwis3Connector
{
public:
    CoordinateSystemConnector(const Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    bool storeMetaData(IlwisObject *data, const IOOptions &options=IOOptions());
    bool storeBinaryData(IlwisObject* );

    static bool canUse(const Ilwis::Resource &resource, const UPCatalogConnector &container);

    IlwisObject *create() const;
    static ConnectorInterface *create(const Resource &resource, bool load=true, const Ilwis::IOOptions &options=IOOptions());

    QString format() const;
private:
    GeodeticDatum *getDatum(Ilwis::IEllipsoid &ellipsoid);
    IProjection getProjection(Ilwis::ConventionalCoordinateSystem *csycc);
    IEllipsoid getEllipsoid();

    static QString prjParam2IlwisName(Projection::ProjectionParamValue parm);
};
}
}

#endif // COORDINATESYSTEMCONNECTOR_H
