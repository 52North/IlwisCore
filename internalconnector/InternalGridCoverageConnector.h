#ifndef GRIDCOVERAGECONNECTOR_H
#define GRIDCOVERAGECONNECTOR_H

namespace Ilwis {

namespace Internal {
class InternalRasterCoverageConnector : public IlwisObjectConnector
{
public:
    InternalRasterCoverageConnector(const Ilwis::Resource &item,bool load=true);

    bool loadMetaData(IlwisObject *data);
    Grid *loadGridData(Ilwis::IlwisObject *) ;
    QString provider() const;

    static ConnectorInterface *create(const Ilwis::Resource &item, bool load=true);

    IlwisObject *create() const;

private:
    IlwisTypes _dataType;
};
}
}

#endif // GRIDCOVERAGECONNECTOR_H
