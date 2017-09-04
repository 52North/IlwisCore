#include "../../core/kernel.h"
#include "../../core/ilwiscontext.h"
#include "../../core/catalog/catalog.h"
#include "../../core/ilwisobjects/ilwisdata.h"
#include "../../core/ilwisobjects/ilwisobject.h"
#include "pythonapi_booleanobject.h"
#include "pythonapi_error.h"

using namespace pythonapi;

BooleanObject::BooleanObject(){
}

void BooleanObject::open(const std::string& url, const std::string& format, const std::string& fnamespace, const IOOptions& options){
}

void BooleanObject::store(const std::string& url, const std::string& format, const std::string& fnamespace, const IOOptions& options){
}

bool BooleanObject::__bool__() const{
    return true;
}

std::string BooleanObject::__str__(){
    return  std::string("true");
}

std::string BooleanObject::name(){
    return  std::string("BooleanObject");
}

void BooleanObject::name(std::string name){
}

bool BooleanObject::isInternal() const{
    return true;
}

std::string BooleanObject::type(){
    return Ilwis::TypeHelper::type2name(itBOOL).toStdString();
}

quint64 BooleanObject::ilwisID() const{
    return i64UNDEF;
}

IlwisTypes BooleanObject::ilwisType(){
    return itBOOL;
}
