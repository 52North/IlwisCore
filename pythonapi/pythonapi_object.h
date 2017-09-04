#ifndef PYTHONAPI_OBJECT_H
#define PYTHONAPI_OBJECT_H

#include "pythonapi_extension.h"

typedef quint64 IlwisTypes;

namespace pythonapi{

    class Object{
    public:
        Object(){}
        virtual ~Object(){}
        virtual bool __bool__() const = 0;
        virtual std::string __str__() = 0;
        virtual IlwisTypes ilwisType() = 0;
    };

}
#endif // PYTHONAPI_OBJECT_H
