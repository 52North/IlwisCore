#ifndef PYTHONAPI_PYOBJECT_H
#define PYTHONAPI_PYOBJECT_H

#include <vector>
#include <string>
#include <limits>

typedef struct _object PyObject;
typedef struct bufferinfo Py_buffer;

#include "pythonapi_qtGNUTypedefs.h"

namespace pythonapi {
    const double rUNDEF = -1e308;
    const long   iUNDEF  = -2147483647L;
    const long long i64UNDEF = std::numeric_limits < qint64 >::min();

    const char* typeName(const PyObject* obj);

    //=======std::exception->PyError==========
    void log(std::string message);
    extern PyObject* ilwisException;
    extern PyObject* invalidObjectException;
    extern PyObject* featureCreationError;
    PyObject* translate_Exception_type(std::exception& e);

    //=======basic types=======================
    PyObject* PyFloatFromDouble(double v);
    PyObject* PyLongFromLongLong(long long v);
    PyObject* PyLongFromUnsignedLongLong(unsigned long long v);
    PyObject* PyUnicodeFromString(const char *u);
    PyObject* PyBoolFromLong(long v);
    PyObject* PyLongFromSize_t(quint32 v);
    bool PyLongCheckExact(const PyObject* obj);
    bool PyFloatCheckExact(const PyObject* obj);
    bool PyUnicodeCheckExact(const PyObject* obj);
    long PyLongAsLong(const PyObject* obj);
    double PyFloatAsDouble(const PyObject* obj);
    std::string PyBytesAsString(const PyObject* obj);

    //======tuple==========================
    PyObject* newPyTuple(int size);
    bool setTupleItem(PyObject *tuple, int i, PyObject* v);
    bool PyTupleCheckExact(const PyObject* obj);
    PyObject* PyTupleGetItem(PyObject* obj, int index);
    int PyTupleSize(PyObject* obj);

    //======list============================
    bool PyListCheckExact(const PyObject* obj);
    PyObject* newPyList(int size);
    PyObject* getListItem(PyObject* list, int index);
    bool setListItem(PyObject* list, int index, PyObject* item);

    //========Py_buffer==========================
    Py_buffer* newPyBuffer(void* buf, int len, int readOnly);

    //=========datetime================================
    //    Return a datetime.date object with the specified year, month and day.
    PyObject* PyDateFromDate(int year, int month, int day);

    //    Return a datetime.datetime object with the specified year, month, day, hour, minute, second and microsecond.
    PyObject* PyDateTimeFromDateAndTime(int year, int month, int day, int hour, int minute, int second, int usecond);

    //    Return a datetime.time object with the specified hour, minute, second and microsecond.
    PyObject* PyTimeFromTime(int hour, int minute, int second, int usecond);

    //    Return a datetime.timedelta object representing the given number of days, seconds and microseconds. Normalization is performed so that the resulting number of microseconds and seconds lie in the ranges documented for datetime.timedelta objects.
    PyObject* PyDeltaFromDSU(int days, int seconds, int useconds);

    bool PyDateCheckExact(const PyObject* obj); // c_type(o) = PyDateTime_Date
    bool PyDateTimeCheckExact(const PyObject* obj); // c_type(o) = PyDateTime_DateTime(PyDateTime_Date)
    bool PyTimeCheckExact(const PyObject* obj); // c_type(o) = PyDateTime_Time
    bool PyDeltaCheckExact(const PyObject* obj); // c_type(o) = PyDateTime_Delta

    // c_type(o) = PyDateTime_Date | PyDateTime_DateTime
    int PyDateTimeGET_YEAR(const void* o);
    int PyDateTimeGET_MONTH(const void* o);
    int PyDateTimeGET_DAY(const void* o);

    // c_type(o) = PyDateTime_DateTime
    int PyDateTimeDATE_GET_HOUR(const void* o);
    int PyDateTimeDATE_GET_MINUTE(const void* o);
    int PyDateTimeDATE_GET_SECOND(const void* o);
    int PyDateTimeDATE_GET_MICROSECOND(const void* o);

    // c_type(o) = PyDateTime_Time
    int PyDateTimeTIME_GET_HOUR(const void* o);
    int PyDateTimeTIME_GET_MINUTE(const void* o);
    int PyDateTimeTIME_GET_SECOND(const void* o);
    int PyDateTimeTIME_GET_MICROSECOND(const void* o);

    // c_type(o) = PyDateTime_Delta
    int PyDateTimeDELTA_GET_DAYS(const void* o);
    int PyDateTimeDELTA_GET_SECONDS(const void* o);
    int PyDateTimeDELTA_GET_MICROSECONDS(const void* o);

    //-------------------------------------------
    double CppFloat2Double(const PyObject* ob);
    long    CppLong2long(PyObject *ob);
    unsigned long long CppULongLong2ulonglong(PyObject* ob);
    std::string CppString2stdString(PyObject* ob);
    double CppTupleElement2Double(PyObject* ob, int index);
    long CppTupleElement2Long(PyObject* ob, int index);
    unsigned long long CppTupleElement2ulonglong(PyObject* ob, int index);
    std::string CppTupleElement2String(PyObject* ob, int index);
    int CppTupleElementCount(PyObject* ob);

    //================PyBuild============================
    PyObject* PyBuildDouble(double value);
    PyObject* PyBuildString(std::string str);

    //===============dictionary=========================
    bool PyDictCheckExact(const PyObject* obj);
    PyObject* PyDictNew();
    int PyDictSetItemString(PyObject *dicti, const char *key, PyObject *val);
    PyObject* PyDictGetItemString(PyObject* dicti, const char* key);

    //====================unicode========================
    PyObject* PyUnicodeAsUTF8(PyObject* obj);

    //===================Memory==========================
    char* PyMalloc();

    bool PyIsNone(PyObject* obj);

} // namespace pythonapi

#endif // PYTHONAPI_PYOBJECT_H
