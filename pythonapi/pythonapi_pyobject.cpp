#include <cmath>
#include "Python.h"

#include "pythonapi_pyobject.h"

#include "pythonapi_error.h"

#include "geos/io/ParseException.h"

#include <typeinfo>

#include "datetime.h"

namespace pythonapi {

    const char* typeName(const PyObject* obj){
        return obj->ob_type->tp_name;
    }

    //=======std::exception->PyError==========
    void log(std::string message){
#ifdef QT_DEBUG
        //PySys_WriteStderr("ILWIS %s \n",message.c_str());
#endif
    }

    PyObject* ilwisException;
    PyObject* invalidObjectException;
    PyObject* featureCreationError;

    PyObject* translate_Exception_type(std::exception& e){
        if (typeid(e) == typeid(std::domain_error)){
            return PyExc_ValueError;
        }else if (typeid(e) == typeid(std::out_of_range)){
            return PyExc_IndexError;
        }else if (typeid(e) == typeid(std::bad_alloc)){
            return PyExc_MemoryError;
        }else if (typeid(e) == typeid(std::bad_cast)){
            return invalidObjectException;
        }else if(typeid(e) == ilwisFeatureCreationError_type_info()){
            return featureCreationError;
        }else if(typeid(e) == ilwisErrorObject_type_info()){
            return ilwisException;
        }else if (typeid(e) == typeid(InvalidObject)){
            return invalidObjectException;
        }else if (typeid(e) == typeid(NotImplementedError)){
            return PyExc_NotImplementedError;
        }else if (typeid(e) == typeid(ImportError)){
            return PyExc_ImportError;
        }else if (typeid(e) == typeid(OSError)){
            return PyExc_OSError;
        }else if(typeid(e) == typeid(StopIteration)){
            return PyExc_StopIteration;
        }else if(typeid(e) == typeid(geos::io::ParseException)){
            return PyExc_SyntaxError;
        }
        return PyExc_Exception;
    }

    //=======basic types=======================

    PyObject* PyFloatFromDouble(double v){
        return PyFloat_FromDouble(v);
    }

    PyObject* PyLongFromLongLong(long long v){
        return PyLong_FromLongLong(v);
    }

    PyObject* PyLongFromUnsignedLongLong(unsigned long long v){
        return PyLong_FromUnsignedLongLong(v);
    }

    PyObject* PyUnicodeFromString(const char *u){
        return PyUnicode_FromString(u);
    }

    PyObject* PyBoolFromLong(long v){
        return PyBool_FromLong(v);
    }

    PyObject* PyLongFromSize_t(quint32 v){
        return PyLong_FromSize_t(v);
    }

    bool PyLongCheckExact(const PyObject* obj){
        return PyLong_CheckExact(const_cast<PyObject*>(obj));
    }

    bool PyFloatCheckExact(const PyObject* obj){
        return PyFloat_CheckExact(const_cast<PyObject*>(obj));
    }

    bool PyUnicodeCheckExact(const PyObject* obj){
        return PyUnicode_CheckExact(const_cast<PyObject*>(obj));
    }

    long PyLongAsLong(const PyObject* obj){
        return PyLong_AsLong(const_cast<PyObject*>(obj));
    }

    double PyFloatAsDouble(const PyObject* obj){
        return PyFloat_AsDouble(const_cast<PyObject*>(obj));
    }

    std::string PyBytesAsString(const PyObject* obj){
        return PyBytes_AsString(PyUnicode_AsUTF8String(const_cast<PyObject*>(obj)));
    }

    //======tuple==========================

    PyObject* newPyTuple(int size){
        return PyTuple_New(size);
    }

    bool setTupleItem(PyObject *tuple, int i, PyObject* v){
        if (v) {
            PyTuple_SET_ITEM(tuple, i, v);   // reference to v stolen
            return true;
        }else{
            Py_DECREF(tuple);
            tuple = NULL;
            return false;
        }
    }

    bool PyTupleCheckExact(const PyObject* obj){
        return PyTuple_CheckExact(const_cast<PyObject*>(obj));
    }

    PyObject* PyTupleGetItem(PyObject* obj, int index){
        return PyTuple_GetItem(obj, index);
    }

    int PyTupleSize(PyObject *obj){
        if(PyTupleCheckExact(obj))
        {
            return PyTuple_Size(obj);
        }
        return iUNDEF;
    }

    //======list=============================
    bool PyListCheckExact(const PyObject* obj){
        return PyList_CheckExact(const_cast<PyObject*>(obj));
    }

    PyObject* newPyList(int size){
        return PyList_New(size);
    }

    PyObject* getListItem(PyObject* list, int index){
        return PyList_GetItem(list, index);
    }

    bool setListItem(PyObject* list, int index, PyObject* item){
        return PyList_SetItem(list, index, item);
    }

    //========Py_buffer==========================

    Py_buffer* newPyBuffer(void* buf, int len, int readOnly){
        Py_buffer* ret =(Py_buffer*)malloc(sizeof *ret);
        if (PyBuffer_FillInfo(ret, NULL,  buf, len, readOnly, PyBUF_WRITEABLE)) {
          throw PyExc_BufferError;
        }
        return ret;
    }

    //=========datetime================================

    PyObject* PyDateFromDate(int year, int month, int day){
        PyDateTime_IMPORT;
        return PyDate_FromDate(year, month, day);
    }

    PyObject* PyDateTimeFromDateAndTime(int year, int month, int day, int hour, int minute, int second, int usecond){
        PyDateTime_IMPORT;
        return PyDateTime_FromDateAndTime(year, month, day, hour, minute, second, usecond);
    }

    PyObject* PyTimeFromTime(int hour, int minute, int second, int usecond){
        PyDateTime_IMPORT;
        return PyTime_FromTime(hour, minute, second, usecond);
    }

    PyObject* PyDeltaFromDSU(int days, int seconds, int useconds){
        PyDateTime_IMPORT;
        return PyDelta_FromDSU(days, seconds, useconds);
    }

    bool PyDateCheckExact(const PyObject* obj){
        PyDateTime_IMPORT;
        return PyDate_CheckExact(const_cast<PyObject*>(obj));
    }

    bool PyDateTimeCheckExact(const PyObject* obj){
        PyDateTime_IMPORT;
        return PyDateTime_CheckExact(const_cast<PyObject*>(obj));
    }

    bool PyDeltaCheckExact(const PyObject* obj){
        PyDateTime_IMPORT;
        return PyDelta_CheckExact(const_cast<PyObject*>(obj));
    }

    bool PyTimeCheckExact(const PyObject* obj){
        PyDateTime_IMPORT;
        return PyTime_CheckExact(const_cast<PyObject*>(obj));
    }

    int PyDateTimeGET_YEAR(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_GET_YEAR(reinterpret_cast<PyDateTime_Date*>(const_cast<void*>(o)));
    }

    int PyDateTimeGET_MONTH(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_GET_MONTH(reinterpret_cast<PyDateTime_Date*>(const_cast<void*>(o)));
    }

    int PyDateTimeGET_DAY(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_GET_DAY(reinterpret_cast<PyDateTime_Date*>(const_cast<void*>(o)));
    }

    int PyDateTimeDATE_GET_HOUR(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_DATE_GET_HOUR(reinterpret_cast<PyDateTime_DateTime*>(const_cast<void*>(o)));
    }

    int PyDateTimeDATE_GET_MINUTE(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_DATE_GET_MINUTE(reinterpret_cast<PyDateTime_DateTime*>(const_cast<void*>(o)));
    }

    int PyDateTimeDATE_GET_SECOND(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_DATE_GET_SECOND(reinterpret_cast<PyDateTime_DateTime*>(const_cast<void*>(o)));
    }

    int PyDateTimeDATE_GET_MICROSECOND(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_DATE_GET_MICROSECOND(reinterpret_cast<PyDateTime_DateTime*>(const_cast<void*>(o)));
    }

    int PyDateTimeTIME_GET_HOUR(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_TIME_GET_HOUR(reinterpret_cast<PyDateTime_Time*>(const_cast<void*>(o)));
    }

    int PyDateTimeTIME_GET_MINUTE(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_TIME_GET_MINUTE(reinterpret_cast<PyDateTime_Time*>(const_cast<void*>(o)));
    }

    int PyDateTimeTIME_GET_SECOND(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_TIME_GET_SECOND(reinterpret_cast<PyDateTime_Time*>(const_cast<void*>(o)));
    }

    int PyDateTimeTIME_GET_MICROSECOND(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_TIME_GET_MICROSECOND(reinterpret_cast<PyDateTime_Time*>(const_cast<void*>(o)));
    }

    int PyDateTimeDELTA_GET_DAYS(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_DELTA_GET_DAYS(reinterpret_cast<PyDateTime_Delta*>(const_cast<void*>(o)));
    }

    int PyDateTimeDELTA_GET_SECONDS(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_DELTA_GET_SECONDS(reinterpret_cast<PyDateTime_Delta*>(const_cast<void*>(o)));
    }

    int PyDateTimeDELTA_GET_MICROSECONDS(const void* o){
        PyDateTime_IMPORT;
        return PyDateTime_DELTA_GET_MICROSECONDS(reinterpret_cast<PyDateTime_Delta*>(const_cast<void*>(o)));
    }

    //-------------------------------------------------------------------

    double CppFloat2Double(const PyObject* ob){
        if (PyFloat_CheckExact(ob)) {
            return PyFloat_AS_DOUBLE(ob);
        }
        return rUNDEF;
    }

    long CppLong2long(PyObject* ob){
        if (PyLong_CheckExact(ob)) {
            return PyLong_AsLong(ob);
        }
        return iUNDEF;
    }

    unsigned long long CppULongLong2ulonglong(PyObject* ob){
        if (PyLong_CheckExact(ob)) {
            return PyLong_AsUnsignedLongLong(ob);
        }
        return i64UNDEF;
    }

    std::string CppString2stdString(PyObject* ob){
        if ( PyUnicode_CheckExact(ob)){
            return std::string(PyUnicode_AsUTF8(ob));
        }
        return "?";
    }

    double CppTupleElement2Double(PyObject* ob, int index){
        if (PyTuple_CheckExact(ob))    {
            PyObject *dobj =  PyTuple_GetItem(ob, index);
            return CppFloat2Double(dobj);
        }
        return rUNDEF;
    }

    long CppTupleElement2Long(PyObject* ob, int index){
        if (PyTuple_CheckExact(ob))    {
            PyObject *lobj =  PyTuple_GetItem(ob, index);
            return CppLong2long(lobj);
        }
        return iUNDEF;
    }
    unsigned long long CppTupleElement2ulonglong(PyObject* ob, int index){
        if (PyTuple_CheckExact(ob))    {
            PyObject *lobj =  PyTuple_GetItem(ob, index);
            return CppULongLong2ulonglong(lobj);
        }
        return iUNDEF;
    }

    std::string CppTupleElement2String(PyObject* ob, int index){
        if (PyTuple_CheckExact(ob))    {
            PyObject *dobj =  PyTuple_GetItem(ob, index);
            return CppString2stdString(dobj);
        }
        return "?";
    }

    int CppTupleElementCount(PyObject* ob){
        return PyTuple_Size(ob);
    }

    //==========================PyBuild====================================

    PyObject* PyBuildDouble(double value){
        return Py_BuildValue("d", value);
    }

    PyObject* PyBuildString(std::string str){
        return Py_BuildValue("s", str.c_str());
    }


    //=======================Dictionary===================================

    bool PyDictCheckExact(const PyObject* obj)
    {
        return PyDict_CheckExact(const_cast<PyObject*>(obj));
    }

    PyObject* PyDictNew()
    {
        return PyDict_New();
    }

    int PyDictSetItemString(PyObject *dicti, const char *key, PyObject *val){
        return PyDict_SetItemString(dicti, key, val);
    }

    PyObject* PyDictGetItemString(PyObject* dicti, const char* key){
        return PyDict_GetItemString(dicti, key);
    }


    //============================PyUnicode=====================================

    PyObject* PyUnicodeAsUTF8(PyObject* obj){
        return PyUnicode_AsUTF8String(obj);
    }


    //============================Memory========================================

    char* PyMalloc(){
        char *buf = (char *) PyMem_Malloc(BUFSIZ);
        return buf;
    }

    bool PyIsNone(PyObject* obj){
        return obj == Py_None;
    }

} // namespace pythonapi
