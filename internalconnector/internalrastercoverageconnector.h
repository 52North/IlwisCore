#ifndef GRIDCOVERAGECONNECTOR_H
#define GRIDCOVERAGECONNECTOR_H

namespace Ilwis {

namespace Internal {
class InternalRasterCoverageConnector : public IlwisObjectConnector
{
public:
    InternalRasterCoverageConnector(const Ilwis::Resource &resource,bool load=true,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    bool loadData(Ilwis::IlwisObject *, const IOOptions& options = IOOptions()) ;
    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true, const Ilwis::IOOptions &options=IOOptions());

    IlwisObject *create() const;

private:
    IlwisTypes _dataType;
};
}
}

#endif // GRIDCOVERAGECONNECTOR_H
