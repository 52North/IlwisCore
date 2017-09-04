#include "../../core/kernel.h"
#include "../../core/ilwisobjects/ilwisobject.h"
#include "../../core/ilwisobjects/ilwisdata.h"
#include "../../core/ilwisobjects/domain/domainitem.h"
#include "../../core/ilwisobjects/domain/domain.h"
#include "../../core/util/range.h"
#include "../../core/ilwisobjects/domain/datadefinition.h"

#include "pythonapi_datadefinition.h"
#include "pythonapi_error.h"
#include "pythonapi_extension.h"
#include "pythonapi_domain.h"
#include "pythonapi_range.h"

#include <regex>

using namespace pythonapi;

DataDefinition::DataDefinition(): _ilwisDatadef(new Ilwis::DataDefinition()){
}

DataDefinition::DataDefinition(Ilwis::DataDefinition* datdef) : _ilwisDatadef(new Ilwis::DataDefinition(*datdef)){
}

DataDefinition::DataDefinition(const Domain& dm, Range* rng){
    if(dm.__bool__() && rng){
        this->_ilwisDatadef.reset(new Ilwis::DataDefinition(dm.ptr()->as<Ilwis::Domain>(), rng->_range->clone()));
    } else if(dm.__bool__()){
        this->_ilwisDatadef.reset(new Ilwis::DataDefinition(dm.ptr()->as<Ilwis::Domain>()));
    }
}

DataDefinition::DataDefinition(const DataDefinition &datdef){
    if(datdef.__bool__())
        this->_ilwisDatadef.reset(new Ilwis::DataDefinition(datdef.ptr()));
}

DataDefinition::~DataDefinition(){
}

std::string DataDefinition::__str__(){
    if(std::regex_match(this->domain()->name(), std::regex("(_ANONYMOUS_)(.*)")))
        return "Domain: " + this->domain()->type()+ ", Range: " + this->range()->__str__();
    else if (domain() && range())
        return "Domain name: " + this->domain()->name()+ ", Domain type: " + this->domain()->type()+ ", Range: " + this->range()->__str__();
    else if (domain() && domain()->__bool__())
        return "Domain name: " + this->domain()->name()+ ", Domain type: " + this->domain()->type();
    else
        throw InvalidObject("use of invalid Domain or Range");
}

bool DataDefinition::__bool__() const{
    return (bool)this->_ilwisDatadef && this->_ilwisDatadef->isValid();
}

void DataDefinition::range(const Range &rng){
    this->ptr().range(rng._range->clone());
}

void DataDefinition::domain(const Domain &dm){
    this->ptr().domain(dm.ptr()->as<Ilwis::Domain>());
}

bool DataDefinition::isCompatibleWith(const DataDefinition& datdef){
    return this->ptr().isCompatibleWith(datdef.ptr());
}

DataDefinition* DataDefinition::merge(const DataDefinition& datdef1, const DataDefinition& datdef2){
    DataDefinition* newDatdef = new DataDefinition();

    Ilwis::DataDefinition ilwdef = Ilwis::DataDefinition::merge(datdef1.ptr(), datdef2.ptr());

    newDatdef->_ilwisDatadef.reset(new Ilwis::DataDefinition(ilwdef));
    return newDatdef;
}

DataDefinition* DataDefinition::__set__(const DataDefinition& def){
    this->ptr() = def.ptr();
    return this;
}

Range* DataDefinition::range() const{
    if(this->ptr().range()){
        return new Range(this->ptr().range()->clone());
    }else
        return NULL;
}

Domain* DataDefinition::domain() const{
    Ilwis::IDomain ilwDom = (this->ptr().domain());
    if(ilwDom.isValid()){
        return new Domain(ilwDom);
    }else
        return NULL;
}

Ilwis::DataDefinition& DataDefinition::ptr() const{
    if (!this->__bool__())
        throw InvalidObject("use of invalid DataDefinition(ptr)");
    return (*this->_ilwisDatadef);
}
