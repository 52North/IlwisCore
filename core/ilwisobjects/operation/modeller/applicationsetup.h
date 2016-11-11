#ifndef APPLICATIONSETUP_H
#define APPLICATIONSETUP_H
#include "kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT ModelApplication : public Identity
{
public:
    ModelApplication();

    void store(QDataStream& stream);
    void load(QDataStream& stream);
    virtual QString type() const = 0;
};

typedef std::shared_ptr<ModelApplication> SPModelApplication;
}

#endif // APPLICATIONSETUP_H
