#ifndef ELLIPSOIDCONNECTOR_H
#define ELLIPSOIDCONNECTOR_H

namespace Ilwis {
namespace Ilwis3{

class EllipsoidConnector : public Ilwis3Connector
{
public:
    EllipsoidConnector(const Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    Ilwis::IlwisObject *create() const;
    static ConnectorInterface *create(const Resource &resource, bool load=true,const IOOptions& options=IOOptions());
private:
    bool createEllipsoidFromCode(const QString &code, IlwisObject *data);
    bool createEllipsoidFromDatum(const QString &name, IlwisObject *data);
};
}
}

#endif // ELLIPSOIDCONNECTOR_H
