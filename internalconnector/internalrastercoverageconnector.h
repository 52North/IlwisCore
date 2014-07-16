#ifndef GRIDCOVERAGECONNECTOR_H
#define GRIDCOVERAGECONNECTOR_H

namespace Ilwis {

namespace Internal {
class InternalRasterCoverageConnector : public IlwisObjectConnector
{
public:
    InternalRasterCoverageConnector(const Ilwis::Resource &resource,bool load=true,const PrepareOptions& options=PrepareOptions());

    bool loadMetaData(IlwisObject *data, const PrepareOptions &options);
    bool loadData(Ilwis::IlwisObject *, const LoadOptions& options = LoadOptions()) ;
    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true, const Ilwis::PrepareOptions &options=PrepareOptions());

    IlwisObject *create() const;

private:
    IlwisTypes _dataType;
};
}
}

#endif // GRIDCOVERAGECONNECTOR_H
