#ifndef ILWISOBJECTCONNECTOR_H
#define ILWISOBJECTCONNECTOR_H


#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT IlwisObjectConnector : public ConnectorInterface
{
public:
    IlwisObjectConnector(const Ilwis::Resource &resource, bool );

    virtual IlwisObject *create() const = 0;
    IlwisTypes type() const;
    Resource& source() ;
    bool binaryIsLoaded() const;

protected:
    template<class T> T setObject(const QString& propeprty, const QString defaultName) {

        T object;
        bool ok;
        auto id = _resource[propeprty].toLongLong(&ok);
        if (!ok)
            return 0;
        Resource resource = mastercatalog()->id2Resource(id);
        if ( resource.code() != sUNDEF)
            object.prepare(id);
        else
            object.prepare(defaultName);

        return object;
    }

    Resource _resource;
    bool _binaryIsLoaded;
    std::mutex _mutex;
protected:
    std::unique_ptr<ContainerConnector>& containerConnector(IlwisObject::ConnectorMode mode= IlwisObject::cmINPUT);
    const std::unique_ptr<ContainerConnector>& containerConnector(IlwisObject::ConnectorMode mode=IlwisObject::cmINPUT) const;

private:
    std::unique_ptr<ContainerConnector> _incontainerconnector;
    std::unique_ptr<ContainerConnector> _outcontainerconnector;
};
}

#endif // ILWISOBJECTCONNECTOR_H
