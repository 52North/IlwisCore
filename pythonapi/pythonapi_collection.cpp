#include "../../core/kernel.h"
#include "pythonapi_engine.h"
#include "pythonapi_collection.h"
#include "pythonapi_error.h"

using namespace pythonapi;

Collection::Collection(std::vector<Object *> items)
: _items(items)
{
}

bool Collection::__bool__() const
{
    return _items.size() > 0;
}

std::string Collection::__str__()
{
    std::string tostr = "";
    for (Object * item : _items) {
        tostr += (tostr.size() > 0) ? ("," + item->__str__()) : item->__str__();
    }
    tostr = "(" + tostr + ")";
    return tostr;
}

IlwisTypes Collection::ilwisType()
{
    return itCOLLECTION;
}

int Collection::__len__() {
    return _items.size();
}

Object* Collection::_getitem(quint32 colIndex) {
    if (colIndex >= _items.size())
        throw std::out_of_range(QString("Index '%1' out of range").arg(colIndex).toStdString());
    else
        return _items[colIndex];
}

Collection* Collection::toCollection(Object *obj){
    Collection* ptr = dynamic_cast<Collection*>(obj);
    if(!ptr)
        throw InvalidObject("cast to Collection not possible");
    return ptr;
}
