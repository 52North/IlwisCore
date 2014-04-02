#ifndef ILWISOBJECTCONNECTOR_H
#define ILWISOBJECTCONNECTOR_H


#include "Kernel_global.h"

namespace Ilwis {
class CatalogConnector;
typedef std::unique_ptr<Ilwis::CatalogConnector> UPCatalogConnector;

class KERNELSHARED_EXPORT IlwisObjectConnector : public ConnectorInterface
{
public:
    IlwisObjectConnector(const Ilwis::Resource &resource, bool,const PrepareOptions& options=PrepareOptions() );
    virtual ~IlwisObjectConnector();

    virtual IlwisObject *create() const = 0;
    IlwisTypes type() const;
    Resource& source() ;
    const Resource &source() const;
    bool dataIsLoaded() const;

protected:
    template<class T> T setObject(const QString& propeprty, const QUrl& defaultName) {

        T object;
        bool ok;
        auto id = _resource[propeprty].toLongLong(&ok);
        if (!ok)
            return 0;
        Resource resource = mastercatalog()->id2Resource(id);
        if ( resource.code() != sUNDEF)
            object.prepare(id);
        else
            object.prepare(defaultName.toString());

        return object;
    }

    Resource _resource;
    bool _binaryIsLoaded;
    std::mutex _mutex;
protected:
    UPCatalogConnector& containerConnector(IlwisObject::ConnectorMode mode= IlwisObject::cmINPUT);
    const UPCatalogConnector& containerConnector(IlwisObject::ConnectorMode mode=IlwisObject::cmINPUT) const;

private:
    UPCatalogConnector _incontainerconnector;
    UPCatalogConnector _outcontainerconnector;
};
}

#endif // ILWISOBJECTCONNECTOR_H
