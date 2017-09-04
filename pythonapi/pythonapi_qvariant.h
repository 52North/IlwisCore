#ifndef PYTHONAPI_QVARIANT_H
#define PYTHONAPI_QVARIANT_H

#include "pythonapi_object.h"
#include <memory>

//Qt typedefs
typedef unsigned int uint;
typedef qint64 qlonglong;
typedef quint64 qulonglong;

class QVariant;

typedef struct _object PyObject;

namespace pythonapi{

    QVariant* PyObject2QVariant(const PyObject* obj);
    PyObject* QVariant2PyObject(const QVariant& var);
    PyObject* StdVectorOfQVariant2PyTuple(const std::vector<QVariant>& vec);
}

#endif // PYTHONAPI_QVARIANT_H
