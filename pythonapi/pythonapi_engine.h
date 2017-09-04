#ifndef PYTHONAPI_ENGINE_H
#define PYTHONAPI_ENGINE_H

#include "pythonapi_object.h"

typedef struct _object PyObject;

namespace pythonapi {
    class Catalog;
    class Engine{
    public:
        Engine();
        static qint64 _do2(std::string output_name, std::string operation,std::string c3 = "",std::string c4 = "",std::string c5 = "",std::string c6 = "",std::string c7="", std::string c8="", std::string c9="");
        static Object* _do(std::string output_name, std::string operation,std::string c3 = "",std::string c4 = "",std::string c5 = "",std::string c6 = "",std::string c7="", std::string c8="", std::string c9="");
        static void setWorkingCatalog(const std::string& location);
        static std::string getLocation();
        static PyObject* operations();
        static std::string operationMetaData(const std::string& name, const std::string &element = "syntax");
        static PyObject* _catalogItems();
    };

}
#endif // PYTHONAPI_ENGINE_H
