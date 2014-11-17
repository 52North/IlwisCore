#ifndef IDENTITYINTERFACE_H
#define IDENTITYINTERFACE_H

#include "kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT IdentityInterface {
public:
    friend class Resource;

    IdentityInterface();
    virtual ~IdentityInterface();
    virtual void code(const QString& code) = 0;
    virtual QString code() const = 0;
    virtual quint64 id() const = 0;
    virtual QString name() const = 0;
    virtual void name(const QString& n) = 0;
    virtual QString description() const = 0;
    virtual void setDescription(const QString& desc) = 0;
};
}



#endif // IDENTITYINTERFACE_H
