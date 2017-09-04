#include <iterator>

#include "../../core/kernel.h"
#include "../../core/ilwisobjects/ilwisdata.h"

#include "../../core/ilwisobjects/coverage/coverage.h"
#include "../../core/ilwisobjects/domain/numericrange.h"
#include "../../core/ilwisobjects/domain/numericdomain.h"
#include "../../core/ilwisobjects/table/columndefinition.h"
#include "../../core/ilwisobjects/table/table.h"
#include "../../core/ilwisobjects/table/attributedefinition.h"

#include "../../core/ilwisobjects/coverage/featurecoverage.h"
#include "../../core/ilwisobjects/coverage/feature.h"
#include "../../core/ilwisobjects/coverage/featureiterator.h"

#include "pythonapi_featureiterator.h"
#include "pythonapi_extension.h"
#include "pythonapi_featurecoverage.h"
#include "pythonapi_error.h"

using namespace pythonapi;

FeatureIterator::FeatureIterator(FeatureCoverage* fc):  _coverage(fc),_ilwisFeatureIterator(new Ilwis::FeatureIterator((*fc->ptr()))){
    this->_end.reset(new Ilwis::FeatureIterator(this->ptr().end()));
}

FeatureIterator::FeatureIterator(const FeatureIterator &it): _coverage(it._coverage),_ilwisFeatureIterator(new Ilwis::FeatureIterator(it.ptr())){
    this->_end.reset(new Ilwis::FeatureIterator(this->ptr().end()));
}

Feature FeatureIterator::__next__(){
    Ilwis::FeatureIterator& iter = this->ptr();
    if (iter != (*this->_end)){
        Ilwis::SPFeatureI test = *iter;
        Ilwis::SPFeatureI* f = new Ilwis::SPFeatureI(*iter);
        iter++;
        return Feature(f, this->_coverage);
    }else{
        throw StopIteration();
    }
}

Feature FeatureIterator::current(){
    return Feature(new Ilwis::SPFeatureI(*this->ptr()), this->_coverage);
}

Feature FeatureIterator::__getitem__(quint32 pos){
    if (pos < _coverage->featureCount()) {
        FeatureIterator positer = FeatureIterator(this->_coverage) + pos;
        return positer.current();
    } else
        return Feature(Ilwis::SPFeatureI(), this->_coverage);
}

FeatureIterator *FeatureIterator::__iter__(){
    return this;
}

std::string FeatureIterator::__str__(){
    if(this->__bool__()){
        return std::string("FeatureIterator for ")+this->_coverage->name();
    }else{
        return "invalid FeatureIterator!";
    }
}

bool FeatureIterator::operator==(const FeatureIterator &other){
    return this->ptr() == other.ptr();
}

bool FeatureIterator::operator!=(const FeatureIterator &other){
    return this->ptr() != other.ptr();
}

FeatureIterator FeatureIterator::operator+(int n){
    FeatureIterator temp(*this);
    temp.ptr() + n;
    return temp;
}

FeatureIterator FeatureIterator::__radd__(int n){
    return this->operator +(n);
}

FeatureIterator FeatureIterator::operator-(int n){
    FeatureIterator temp(*this);
    temp.ptr() - n;
    return temp;
}

bool FeatureIterator::__bool__() const{
    return (this->_coverage && this->_coverage->__bool__() && (bool)this->_ilwisFeatureIterator);
}

//FeatureIterator FeatureIterator::end(){
//    FeatureIterator newIter = FeatureIterator(*this);
//    newIter._ilwisFeatureIterator.reset(new Ilwis::FeatureIterator(newIter.ptr().end()));
//    return newIter;
//}

Ilwis::FeatureIterator& FeatureIterator::ptr() const{
    if (!this->__bool__())
        throw InvalidObject("use of invalid FeatureIterator (ptr)");
    return (*this->_ilwisFeatureIterator);
}
