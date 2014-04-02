#ifndef INTERNALTABLECONNECTOR_H
#define INTERNALTABLECONNECTOR_H

namespace Ilwis {
namespace Internal {

class InternalTableConnector : public IlwisObjectConnector
{
public:
    InternalTableConnector(const Resource &resource, bool load,const PrepareOptions& options=PrepareOptions());

    bool loadMetaData(IlwisObject* data,const PrepareOptions&);
    QString type() const;
    virtual IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const PrepareOptions& options=PrepareOptions());
    bool loadData(IlwisObject *);
    QString provider() const;
};
}
}

#endif // INTERNALTABLECONNECTOR_H
