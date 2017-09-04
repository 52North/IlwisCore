#include "../../core/kernel.h"
#include "../../core/ilwisobjects/ilwisobject.h"
#include "../../core/ilwisobjects/ilwisdata.h"
#include "../../core/ilwisobjects/domain/domainitem.h"
#include "../../core/ilwisobjects/domain/domain.h"
#include "../../core/util/range.h"
#include "../../core/ilwisobjects/domain/datadefinition.h"
#include "../../core/ilwisobjects/table/columndefinition.h"

#include "pythonapi_columndefinition.h"
#include "pythonapi_datadefinition.h"
#include "pythonapi_domain.h"
#include "pythonapi_range.h"
#include "pythonapi_error.h"
#include "pythonapi_extension.h"

using namespace pythonapi;

ColumnDefinition::ColumnDefinition() : _ilwisColDef(new Ilwis::ColumnDefinition()){
}

ColumnDefinition::ColumnDefinition(const ColumnDefinition& coldef, quint32 index){
    if(coldef.__bool__())
    {
        Ilwis::ColumnDefinition* ilwDef = coldef.ptr().get();
        if(ilwDef->isValid())
            _ilwisColDef.reset(new Ilwis::ColumnDefinition(*ilwDef, index));
    }
}

ColumnDefinition::ColumnDefinition(const std::string& name, const DataDefinition& datdef, quint64 colindex){
    if(datdef.__bool__())
        _ilwisColDef.reset(new Ilwis::ColumnDefinition(QString::fromStdString(name), datdef.ptr(), colindex));
}

ColumnDefinition::ColumnDefinition(Ilwis::ColumnDefinition* colDef){
    if(colDef->isValid())
        _ilwisColDef.reset(colDef);
}

ColumnDefinition::ColumnDefinition(const ColumnDefinition& coldef){
    if(coldef.__bool__())
    {
        Ilwis::ColumnDefinition* ilwDef = coldef.ptr().get();
        if(ilwDef->isValid())
            _ilwisColDef.reset(new Ilwis::ColumnDefinition(*ilwDef));
    }
}

ColumnDefinition::ColumnDefinition(const std::string& name, const Domain &dom, quint64 colindex){
    DataDefinition datdef = DataDefinition(dom);
    _ilwisColDef.reset(new Ilwis::ColumnDefinition(QString::fromStdString(name), datdef.ptr(), colindex));
}

ColumnDefinition::~ColumnDefinition(){

}

DataDefinition &ColumnDefinition::datadef(){
    Ilwis::DataDefinition ilwDef (this->ptr()->datadef());
    DataDefinition* pyDef = new DataDefinition(&ilwDef);
    return *pyDef;
}

void ColumnDefinition::columnindex(quint64 idx){
    this->ptr()->columnindex(idx);
}

quint64 ColumnDefinition::columnindex() const{
    return this->ptr()->columnindex();
}

bool ColumnDefinition::isChanged() const{
    return this->ptr()->isChanged();
}

void ColumnDefinition::changed(bool yesno){
    this->ptr()->changed(yesno);
}

bool ColumnDefinition::__bool__() const{
    return (bool)this->_ilwisColDef && this->_ilwisColDef->isValid();
}

std::string ColumnDefinition::__str__(){
    return "Name: " + this->ptr()->name().toStdString() + " Columnindex: " +
            std::to_string(columnindex()) + " "  + datadef().__str__();
}

std::shared_ptr<Ilwis::ColumnDefinition> ColumnDefinition::ptr() const{
    if (!this->__bool__())
        throw InvalidObject("use of invalid ColumnDefinition(ptr)");
    return this->_ilwisColDef;
}
