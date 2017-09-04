#ifndef ILWIS3REPRESENTATIONCONNECTOR_H
#define ILWIS3REPRESENTATIONCONNECTOR_H

namespace Ilwis {
namespace Ilwis3 {
class Ilwis3RepresentationConnector : public Ilwis3Connector
{
public:
     ~Ilwis3RepresentationConnector();

    Ilwis3RepresentationConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    bool storeMetaData(IlwisObject *obj, const Ilwis::IOOptions &options);
    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    QString format() const;

private:
    bool parseRprClass();
    bool parseRprGradual(Ilwis::Representation *rpr);
};
}
}

#endif // ILWIS3REPRESENTATIONCONNECTOR_H
