#ifndef COVERAGECONNECTOR_H
#define COVERAGECONNECTOR_H

namespace Ilwis {

class Coverage;

namespace Gdal {

class CoverageConnector : public GdalConnector
{
public:
    CoverageConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadMetaData(Ilwis::IlwisObject *data, const IOOptions &options);

protected:
    bool store(IlwisObject *obj, const IOOptions& options = IOOptions());

    OGRSpatialReferenceH createSRS(const ICoordinateSystem &coordsystem) const;

private:
    //void setCsy(Coverage *coverage);
};
}
}

#endif // COVERAGECONNECTOR_H
