#ifndef PROJECTIONCONNECTOR_H
#define PROJECTIONCONNECTOR_H

namespace Ilwis {
namespace Ilwis3 {
class ProjectionConnector : public Ilwis3Connector
{
public:
    ProjectionConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
private:
    Projection::ProjectionParamValue mapKeyToEnum(const QString &key) const;
    QString _internalCode;
};
}
}

#endif // PROJECTIONCONNECTOR_H
