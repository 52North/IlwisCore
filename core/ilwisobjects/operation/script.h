#ifndef SCRIPT_H
#define SCRIPT_H

#include "kernel_global.h"

namespace Ilwis {


class KERNELSHARED_EXPORT Script : public OperationMetaData
{
public:
    Script();
    Script(const Resource& resource);

    QString text() ;
    void text(const QString& txt);

    IlwisTypes ilwisType() const;
private:
    QString _text;


};

typedef Ilwis::IlwisData<Ilwis::Script> IScript;
}

#endif // SCRIPT_H
