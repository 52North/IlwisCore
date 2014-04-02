#ifndef INTERNALFEATURECOVERAGECONNECTOR_H
#define INTERNALFEATURECOVERAGECONNECTOR_H

namespace Ilwis {

namespace Internal {
class InternalFeatureCoverageConnector : public IlwisObjectConnector
{
public:
    InternalFeatureCoverageConnector(const Ilwis::Resource &resource,bool load=true,const PrepareOptions& options=PrepareOptions());

    bool loadMetaData(IlwisObject *data, const PrepareOptions &options);
    bool loadData(Ilwis::IlwisObject *) ;
    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());

    IlwisObject *create() const;

private:
    IlwisTypes _dataType;
};
}
}

#endif // INTERNALFEATURECOVERAGECONNECTOR_H
