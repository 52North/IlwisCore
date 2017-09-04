#ifndef GEOREFCONNECTOR_H
#define GEOREFCONNECTOR_H

namespace Ilwis {
namespace Ilwis3 {
class GeorefConnector : public Ilwis3Connector
{
public:
    GeorefConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    bool storeMetaData(IlwisObject *obj, const IOOptions &options);
    IlwisObject *create() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());

    QString format() const;
private:
    bool loadGeoref(const IniFile &odf, IlwisObject *data);
    void createGeoreference(const IniFile &odf, GeoReference *grf) const;
    bool loadGeorefCorners(const IniFile &odf, Ilwis::IlwisObject *data);
    bool loadGeorefTiepoints(const IniFile &odf, GeoReference *grf);
};
}
}

#endif // GEOREFCONNECTOR_H
