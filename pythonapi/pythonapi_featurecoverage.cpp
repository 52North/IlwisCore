#include "../../core/kernel.h"
#include "../../core/ilwiscontext.h"
#include "../../core/catalog/catalog.h"
#include "../../core/ilwisobjects/ilwisobject.h"

#include "../../core/util/ilwisangle.h"

#include "../../core/util/geometries.h"
#include "../../core/util/box.h"

#include "../../core/ilwisobjects/ilwisdata.h"

#include "../../core/ilwisobjects/domain/domain.h"
#include "../../core/ilwisobjects/domain/datadefinition.h"
#include "../../core/ilwisobjects/table/columndefinition.h"
#include "../../core/ilwisobjects/table/table.h"
#include "../../core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.h"
#include "../../core/ilwisobjects/table/attributedefinition.h"

#include "geos/geom/Geometry.h"
#include "geos/geom/CoordinateFilter.h"

#include "../../core/ilwisobjects/coverage/coverage.h"
#include "../../core/ilwisobjects/coverage/featurecoverage.h"
#include "../../core/ilwisobjects/coverage/feature.h"

#include "../../core/ilwisobjects/coverage/featureiterator.h"
#include "../../core/ilwisobjects/geometry/coordinatesystem/csytransform.h"
#include "../../core/ilwisobjects/coverage/geometryhelper.h"

#include "pythonapi_featurecoverage.h"
#include "pythonapi_domain.h"
#include "pythonapi_error.h"
#include "pythonapi_pyobject.h"
#include "pythonapi_qvariant.h"

using namespace pythonapi;

FeatureCoverage::FeatureCoverage(const Ilwis::IFeatureCoverage &coverage):Coverage(Ilwis::ICoverage(coverage)){
}

FeatureCoverage::FeatureCoverage(){
    Ilwis::IFeatureCoverage fc;
    fc.prepare();
    if (fc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(fc));
}

FeatureCoverage::FeatureCoverage(const std::string& resource){
    QString input (QString::fromStdString(resource));
    input.replace('\\','/');
    // if it is file:// (or http:// etc) leave it untouched; if not, append file:// and the working catalog path if it is missing
    if (input.indexOf("://") < 0) {
        int pos = input.indexOf('/');
        if (pos > 0) {
            if ((input.count('/') > 1) || QFileInfo(input).exists()) // full path starting with drive-letter (MS-DOS-style)
                input = "file:///" + input;
            else // container object without path, e.g. myfile.hdf/subdataset: look for it in workingCatalog()
                input = "file:///" + Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
        }  else if (pos == 0) // full path starting with path-separator (UNIX-style)
            input = "file://" + input;
        else { // pos < 0: file without path, or new object
            QString file = Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
            if (QFileInfo (file).exists()) // file without path
                input = "file:///" + file;
        }
    }
    Ilwis::IFeatureCoverage fc(input, itFEATURE);
    if (fc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(fc));
}

FeatureIterator FeatureCoverage::__iter__(){
    return FeatureIterator(this);
}

IlwisTypes FeatureCoverage::featureTypes() const
{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->featureTypes();
}

void FeatureCoverage::featureTypes(IlwisTypes type)
{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->featureTypes(type);
}

unsigned int FeatureCoverage::featureCount() const{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->featureCount();
}

void FeatureCoverage::setFeatureCount(IlwisTypes type, quint32 geomCnt){
    this->ptr()->as<Ilwis::FeatureCoverage>()->setFeatureCount(type, geomCnt, 0);
}

Feature FeatureCoverage::newFeature(const std::string& wkt, const CoordinateSystem& csy, bool load){
    Ilwis::SPFeatureI ilwFeatureI = this->ptr()->as<Ilwis::FeatureCoverage>()->newFeature(QString::fromStdString(wkt), csy.ptr()->as<Ilwis::CoordinateSystem>(), load);
    return Feature(ilwFeatureI, this);
}

Feature FeatureCoverage::newFeature(const Geometry &geometry){
    Ilwis::SPFeatureI ilwFeatureI =this->ptr()->as<Ilwis::FeatureCoverage>()->newFeature(geometry.ptr().get()->clone());
    return Feature(ilwFeatureI, this);
}

Feature FeatureCoverage::newFeatureFrom(const Feature& feat, const CoordinateSystem& csy){
    Ilwis::FeatureInterface* ilwFeat = feat.ptr().get()->clone(this->ptr()->as<Ilwis::FeatureCoverage>().ptr());
    Ilwis::SPFeatureI ilwFeatureI = this->ptr()->as<Ilwis::FeatureCoverage>()->newFeatureFrom(ilwFeat, csy.ptr()->as<Ilwis::CoordinateSystem>());
    return Feature(ilwFeatureI ,this);
}

Table FeatureCoverage::attributeTable(){
    Ilwis::ITable ilwTab = this->ptr()->as<Ilwis::FeatureCoverage>()->attributeTable();
    return Table(Ilwis::ITable(ilwTab));
}

void FeatureCoverage::attributesFromTable(const Table &otherTable){
    this->ptr()->as<Ilwis::FeatureCoverage>()->setAttributes(otherTable.ptr()->as<Ilwis::Table>());
}

void FeatureCoverage::addColumn(const ColumnDefinition &coldef){
    Ilwis::ColumnDefinition ilwDef = *(coldef.ptr());
    if (!this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().addColumn(ilwDef))
        throw Ilwis::ErrorObject(QString("Could not add column '%1' of domain '%2' to the list of columns").arg(ilwDef.name()).arg(ilwDef.datadef().domain()->name()));
}

void FeatureCoverage::addColumn(const std::string &name, const std::string &domainname){
    if (!this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().addColumn(QString::fromStdString(name), QString::fromStdString(domainname)))
        throw Ilwis::ErrorObject(QString("Could not add column '%1' of domain '%2' to the list of columns").arg(QString::fromStdString(name)).arg(QString::fromStdString(domainname)));
}

ColumnDefinition FeatureCoverage::columndefinition(const std::string &nme) const{
    Ilwis::ColumnDefinition ilwDef = this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().columndefinition(QString::fromStdString(nme));
    return ColumnDefinition(new Ilwis::ColumnDefinition(ilwDef));
}

ColumnDefinition FeatureCoverage::columndefinition(quint32 index) const{
    Ilwis::ColumnDefinition ilwDef = this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().columndefinition(index);
    return ColumnDefinition(new Ilwis::ColumnDefinition(ilwDef));
}

void FeatureCoverage::setColumndefinition(const ColumnDefinition &coldef){
    Ilwis::ColumnDefinition ilwDef = *(coldef.ptr());
    this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().columndefinition(ilwDef);
}

quint32 FeatureCoverage::columnIndex(const std::string &nme) const{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().columnIndex(QString::fromStdString(nme));
}

ColumnDefinition FeatureCoverage::__getitem__(quint32 index){
    Ilwis::ColumnDefinition ilwDef = this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef()[index];
    return ColumnDefinition(new Ilwis::ColumnDefinition(ilwDef));
}

PyObject* FeatureCoverage::checkInput(PyObject* inputVar, quint32 columnIndex) const{
    QVariant* qInput = PyObject2QVariant(inputVar);
    QVariant qVar = this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().checkInput(*qInput, columnIndex);
    delete qInput;
    return QVariant2PyObject(qVar);
}

quint32 FeatureCoverage::definitionCount() const{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().definitionCount();
}

void FeatureCoverage::setStackDefinition(const Domain& dom, PyObject* items){
    if(PyTupleCheckExact(items)){
        int sz = PyTupleSize(items);
        if(PyFloatCheckExact(PyTupleGetItem(items, 0))){
            std::vector<double> ilwVec;
            for(int i = 0; i < sz; ++i){
                double val = PyFloatAsDouble(PyTupleGetItem(items, i));
                ilwVec.push_back(val);
            }
            this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().setSubDefinition(dom.ptr()->as<Ilwis::Domain>(), ilwVec);
        }else if(PyLongCheckExact(PyTupleGetItem(items, 0))){
            std::vector<double> ilwVec;
            for(int i = 0; i < sz; ++i){
                long val = PyLongAsLong(PyTupleGetItem(items, i));
                ilwVec.push_back((double)val);
            }
            this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().setSubDefinition(dom.ptr()->as<Ilwis::Domain>(), ilwVec);
        }else if(PyUnicodeCheckExact(PyTupleGetItem(items, 0))){
            std::vector<QString> ilwVec;
            for(int i = 0; i < sz; ++i){
                std::string val = PyBytesAsString(PyTupleGetItem(items, i));
                ilwVec.push_back(QString::fromStdString(val));
            }
            this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().setSubDefinition(dom.ptr()->as<Ilwis::Domain>(), ilwVec);
        }else if(PyDateTimeCheckExact(PyTupleGetItem(items, 0)) || PyDateCheckExact(PyTupleGetItem(items, 0)) || PyTimeCheckExact(PyTupleGetItem(items, 0))){
            std::vector<QString> ilwVec;
            for(int i = 0; i < sz; ++i){
                int year = PyDateTimeGET_YEAR(PyTupleGetItem(items, i));
                int month = PyDateTimeGET_MONTH(PyTupleGetItem(items, i));
                int day = PyDateTimeGET_DAY(PyTupleGetItem(items, i));
                std::string dateStr = std::to_string(year) + std::to_string(month) + std::to_string(day);
                ilwVec.push_back(QString::fromStdString(dateStr));
            }
            this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().setSubDefinition(dom.ptr()->as<Ilwis::Domain>(), ilwVec);
        }
    }
}

quint32 FeatureCoverage::indexOf(const std::string& variantId) const{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().index(QString::fromStdString(variantId));
}

quint32 FeatureCoverage::indexOf(double domainItem) const{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().index(domainItem);
}

quint32 FeatureCoverage::indexOf(PyObject* obj) const{
    if(PyDateTimeCheckExact(obj) || PyDateCheckExact(obj) || PyTimeCheckExact(obj)){
        int year = PyDateTimeGET_YEAR(obj);
        int month = PyDateTimeGET_MONTH(obj);
        int day = PyDateTimeGET_DAY(obj);
        std::string dateStr = std::to_string(year) + std::to_string(month) + std::to_string(day);
        return this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().index(QString::fromStdString(dateStr));
    }
    return iUNDEF;
}

std::string FeatureCoverage::atIndex(quint32 idx) const{
    QString qStr =  this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().index(idx);
    return qStr.toStdString();
}

PyObject* FeatureCoverage::indexes() const{
    std::vector<QString> qVec = this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().indexes();
    PyObject* pyTup = newPyTuple(qVec.size());

    for(int i = 0; i < qVec.size(); ++i){
        std::string actStr = qVec[i].toStdString();
        setTupleItem(pyTup, i, PyBuildString(actStr));
    }

    return pyTup;
}

quint32 FeatureCoverage::countStackDomainItems() const{
    return this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().count();
}

Domain FeatureCoverage::stackDomain() const{
    Ilwis::IDomain ilwDom =  this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().domain();
    return Domain(Ilwis::IDomain(ilwDom));
}

void FeatureCoverage::clear(){
    this->ptr()->as<Ilwis::FeatureCoverage>()->attributeDefinitionsRef().clearSubFeatureDefinitions();
}

FeatureCoverage *FeatureCoverage::toFeatureCoverage(Object *obj){
    FeatureCoverage* ptr = dynamic_cast<FeatureCoverage*>(obj);
    if(!ptr)
        throw InvalidObject("cast to FeatureCoverage not possible");
    return ptr;
}

PyObject* FeatureCoverage::select(const std::string& spatialQuery){
    std::vector<quint32> vec = this->ptr()->as<Ilwis::FeatureCoverage>()->select(QString::fromStdString(spatialQuery));
    PyObject* pyTup = newPyTuple(vec.size());
    for(int i = 0; i < vec.size(); i++){
        setTupleItem(pyTup, i, PyLongFromUnsignedLongLong(vec[i]));
    }
    return pyTup;
}

void FeatureCoverage::reprojectFeatures(const CoordinateSystem& csy){
    Ilwis::ICoordinateSystem ilwCsy = csy.ptr()->as<Ilwis::CoordinateSystem>();
    if ( ilwCsy.isValid() && !ilwCsy->isEqual(coordinateSystem().ptr()->as<Ilwis::CoordinateSystem>().ptr())) {
        Ilwis::IFeatureCoverage fc = this->ptr()->as<Ilwis::FeatureCoverage>();
        for(const auto &feat : fc ){
            const Ilwis::UPGeometry& geom = feat->geometry();
            if(!geom)
                continue;
            Ilwis::CsyTransform trans(coordinateSystem().ptr()->as<Ilwis::CoordinateSystem>(), ilwCsy);
            geom->apply_rw(&trans);
            geom->geometryChangedAction();
            Ilwis::GeometryHelper::setCoordinateSystem(geom.get(), ilwCsy.ptr());
        }
        Ilwis::ICoordinateSystem oldCsy = coordinateSystem().ptr()->as<Ilwis::CoordinateSystem>();
        Ilwis::Envelope newEnv = ilwCsy->convertEnvelope(oldCsy, fc->envelope());
        fc->coordinateSystem(ilwCsy);
        fc->envelope(newEnv);
    }
}

FeatureCoverage *FeatureCoverage::clone(){
    Ilwis::IFeatureCoverage ilwFc = this->ptr()->as<Ilwis::FeatureCoverage>()->clone();
    return new FeatureCoverage(ilwFc);
}

IlwisTypes FeatureCoverage::geometryType(const Geometry& geom){
    return this->ptr()->as<Ilwis::FeatureCoverage>()->geometryType(geom.ptr().get());
}

void FeatureCoverage::setCoordinateSystem(const CoordinateSystem &cs){
    this->ptr()->as<Ilwis::FeatureCoverage>()->coordinateSystem(cs.ptr()->as<Ilwis::CoordinateSystem>());
}

const QString FeatureCoverage::getStoreFormat() const {
    return "vectormap";
}

