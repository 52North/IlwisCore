#ifndef WFSFEATURECONNECTOR_H
#define WFSFEATURECONNECTOR_H

#include "wfsconnector_global.h"

#include "kernel.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"

#include "wfsparsingcontext.h"

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsFeatureConnector : public IlwisObjectConnector
{
public:
    static ConnectorInterface *create(const Resource &resource, bool load=true, const IOOptions &options=IOOptions());
    WfsFeatureConnector(const Resource &resource, bool load=true, const IOOptions &options=IOOptions());

    QString provider() const;
    IlwisObject *create() const;
    static IlwisTypes ilwisType(const QString &name);

protected:
    bool loadMetaData(IlwisObject* data, const IOOptions&options);
    bool loadData(IlwisObject* data,const IOOptions& options);

private:
    WfsParsingContext _context;
};
}
}
#endif // WFSFEATURECONNECTOR_H


