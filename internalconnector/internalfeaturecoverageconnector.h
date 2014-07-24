#ifndef INTERNALFEATURECOVERAGECONNECTOR_H
#define INTERNALFEATURECOVERAGECONNECTOR_H

namespace Ilwis {

namespace Internal {
class InternalFeatureCoverageConnector : public IlwisObjectConnector
{
public:
    InternalFeatureCoverageConnector(const Ilwis::Resource &resource,bool load=true,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    bool loadData(Ilwis::IlwisObject *, const Ilwis::IOOptions &options = IOOptions()) ;
    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());

    IlwisObject *create() const;

private:
    IlwisTypes _dataType;
};
}
}

#endif // INTERNALFEATURECOVERAGECONNECTOR_H
