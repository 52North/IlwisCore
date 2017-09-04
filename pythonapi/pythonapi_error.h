#ifndef PYTHONAPI_ERROR_H
#define PYTHONAPI_ERROR_H

#include <stdexcept>

#include "pythonapi_qtGNUTypedefs.h"

//predeclaration of PyObject from object.h
typedef struct _object PyObject;

namespace pythonapi{

    //to indicate an error caused by the attempt to use an invalid (ilwis) object
    class InvalidObject : public std::exception{
        private:
            std::string _what;
        public:
            InvalidObject(const std::string& what );
            virtual const char* what() const throw();
    };

    //to indicate an error during module initialization
    class ImportError : public std::exception{
        private:
            std::string _what;
        public:
            ImportError(const std::string& what );
            virtual const char* what() const throw();
    };

    //to indicate an error during operating system interaction (like IO error)
    class OSError : public std::exception{
        private:
            std::string _what;
        public:
            OSError(const std::string& what );
            virtual const char* what() const throw();
    };

    //to indicate that this ilwis functionality is not yet implemented
    class NotImplementedError : public std::exception{
        private:
            std::string _what;
        public:
            NotImplementedError(const std::string& what );
            virtual const char* what() const throw();
    };

    //to indicate next() == it.end() the Python way during for(each) loops
    class StopIteration : public std::exception{
        public:
            StopIteration(){}
            virtual const char* what() const throw() {return "StopIteration";}
    };

    //can retrieve Ilwis::ErrorObject::message() or std::exception::what()
    const char *get_err_message(std::exception& e);
    //returns typeid(Ilwis::ErrorObject) to compare in translate_Exception_type
    const std::type_info& ilwisErrorObject_type_info();
    const std::type_info& ilwisFeatureCreationError_type_info();

}

#endif // PYTHONAPI_ERROR_H
