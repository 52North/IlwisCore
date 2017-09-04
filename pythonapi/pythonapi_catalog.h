#ifndef PYTHONAPI_CATALOG_H
#define PYTHONAPI_CATALOG_H

#include "pythonapi_ilwisobject.h"
/*
namespace Ilwis {
    class ICatalog;
}
*/

typedef struct _object PyObject;

namespace pythonapi {

    class Catalog : public Object{
        public:
            Catalog(const std::string& url, const std::string& filter = "");
            ~Catalog();

            bool __bool__() const;
            std::string __str__();
            IlwisTypes ilwisType();

            PyObject* items();
            Object* _getitem(const std::string &name);

            static Catalog* toCatalog(Object* obj);
    protected:
            Catalog(Ilwis::ICatalog & cat);
            Ilwis::ICatalog _catalog;
    };

} // namespace pythonapi

#endif // PYTHONAPI_CATALOG_H
