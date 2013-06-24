#ifndef ILWISOBJECTCONNECTOR_H
#define ILWISOBJECTCONNECTOR_H


#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT IlwisObjectConnector : public ConnectorInterface
{
public:
    IlwisObjectConnector(const Ilwis::Resource &item, bool );

    virtual IlwisObject *create() const = 0;
    IlwisTypes type() const;
    Resource& source() ;

protected:
    template<class T> T setObject(const QString& propeprty, const QString defaultName) {

        T object;
        auto id = _resource[propeprty].toLongLong();
        Resource resource = mastercatalog()->id2Resource(id);
        if ( resource.code() != sUNDEF)
            object.prepare(id);
        else
            object.prepare(defaultName);

        return object;
    }

    Resource _resource;
    std::mutex _mutex;
};
}

#endif // ILWISOBJECTCONNECTOR_H
