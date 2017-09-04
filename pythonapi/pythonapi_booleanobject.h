#ifndef PYTHONAPI_BOOLEANOBJECT_H
#define PYTHONAPI_BOOLEANOBJECT_H

#include <memory>
#include "pythonapi_ilwisobject.h"
#include "pythonapi_util.h"

namespace pythonapi {

    class BooleanObject: public IlwisObject{
        friend class Engine;
        friend class Catalog;
    public:
        BooleanObject();

    public:
        void open( const std::string& url,  const std::string& format  = "",  const std::string& fnamespace = "", const pythonapi::IOOptions& options = pythonapi::IOOptions());
        void store( const std::string& url,  const std::string& format  = "",  const std::string& fnamespace = "", const pythonapi::IOOptions& options = pythonapi::IOOptions());
        bool __bool__() const;
        std::string __str__();
        std::string name();
        void name(std::string name);
        bool isInternal() const;
        std::string type();
        quint64 ilwisID() const;
        IlwisTypes ilwisType();
    };

}

#endif // PYTHONAPI_BOOLEANOBJECT_H
