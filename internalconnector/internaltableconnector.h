#ifndef INTERNALTABLECONNECTOR_H
#define INTERNALTABLECONNECTOR_H

namespace Ilwis {
namespace Internal {

class InternalTableConnector : public IlwisObjectConnector
{
public:
    InternalTableConnector(const Resource &resource, bool load);

    bool loadMetaData(IlwisObject* data);
    QString type() const;
    virtual IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true);
    bool loadBinaryData(IlwisObject *);
    QString provider() const;
};
}
}

#endif // INTERNALTABLECONNECTOR_H
