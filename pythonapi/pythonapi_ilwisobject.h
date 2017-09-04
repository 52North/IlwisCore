#ifndef PYTHONAPI_ILWISOBJECT_H
#define PYTHONAPI_ILWISOBJECT_H

#include <memory>
#include "pythonapi_object.h"
#include "pythonapi_util.h"

namespace Ilwis {
    class IlwisObject;

    template<class T> class IlwisData;
    typedef IlwisData<IlwisObject> IIlwisObject;
}

namespace pythonapi {

    class IlwisObject: public Object{
        friend class Engine;
        friend class Catalog;
    public:
        //should be the same as enum Ilwis::IlwisObject::ConnectorMode (ilwisobject.h)
        enum ConnectorMode{cmINPUT=1, cmOUTPUT=2, cmEXTENDED=4};
        enum StoreMode{smMETADATA=1, smBINARYDATA=2};

    protected:
        IlwisObject();
        IlwisObject(Ilwis::IIlwisObject* object);
        std::shared_ptr<Ilwis::IIlwisObject> _ilwisObject;
        std::shared_ptr<Ilwis::IIlwisObject> ptr() const;
        virtual const QString getStoreFormat() const;
    public:
        virtual ~IlwisObject();

        void open( const std::string& url,  const std::string& format  = "",  const std::string& fnamespace = "", const pythonapi::IOOptions& options = pythonapi::IOOptions());
        void store( const std::string& url,  const std::string& format  = "",  const std::string& fnamespace = "", const pythonapi::IOOptions& options = pythonapi::IOOptions());
        bool __bool__() const;
        std::string __str__();
        std::string __add__(std::string value);
        std::string __radd__(std::string value);
        std::string name();
        void name(std::string name);
        bool isInternal() const;
        std::string type();
        std::string type2Name(IlwisTypes ilwType);
        quint64 ilwisID() const;
        IlwisTypes ilwisType();
    };

}

#endif // PYTHONAPI_ILWISOBJECT_H
