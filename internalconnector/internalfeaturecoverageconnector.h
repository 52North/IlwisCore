#ifndef INTERNALFEATURECOVERAGECONNECTOR_H
#define INTERNALFEATURECOVERAGECONNECTOR_H

namespace Ilwis {

namespace Internal {
class InternalFeatureCoverageConnector : public IlwisObjectConnector
{
public:
    InternalFeatureCoverageConnector(const Ilwis::Resource &resource,bool load=true);

    bool loadMetaData(IlwisObject *data);
    bool loadBinaryData(Ilwis::IlwisObject *) ;
    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);

    IlwisObject *create() const;

private:
    IlwisTypes _dataType;
};
}
}

#endif // INTERNALFEATURECOVERAGECONNECTOR_H
