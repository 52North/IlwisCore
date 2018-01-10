#include "../../core/kernel.h"
#include "../../core/ilwiscontext.h"
#include "../../core/catalog/catalog.h"
#include "../../core/ilwisobjects/ilwisdata.h"
#include "../../core/ilwisobjects/ilwisobject.h"
#include "pythonapi_ilwisobject.h"
#include "pythonapi_error.h"

using namespace pythonapi;

IlwisObject::IlwisObject(){
}

IlwisObject::IlwisObject(Ilwis::IIlwisObject *object): _ilwisObject(std::shared_ptr<Ilwis::IIlwisObject>(object)){
}

IlwisObject::~IlwisObject(){
}

void IlwisObject::open(const std::string& url, const std::string& format, const std::string& fnamespace, const IOOptions& options){
    QString input (QString::fromStdString(url));
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
        else { // pos < 0: file without path
            input = "file:///" + Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
        }
    }
    (*this->ptr())->connectTo(QUrl(input), QString::fromStdString(format), QString::fromStdString(fnamespace), Ilwis::IlwisObject::ConnectorMode::cmINPUT, options.ptr());
}

void IlwisObject::store(const std::string& url, const std::string& format, const std::string& fnamespace, const IOOptions& options){
    QString output (QString::fromStdString(url));
    output.replace('\\','/');
    // if it is file:// (or http:// etc) leave it untouched; if not, append file:// and the working catalog path if it is missing
    if (output.indexOf("://") < 0) {
        int pos = output.indexOf('/');
        if (pos > 0) // full path starting with drive-letter (MS-DOS-style)
            output = "file:///" + output;
        else if (pos == 0) // full path starting with path-separator (UNIX-style)
            output = "file://" + output;
        else // file without path
            output = "file:///" + Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + output;
    }
    const std::vector<std::string> ilwis3formats = {"vectormap", "map", "georef", "georeference", "domain", "coordsystem", "table", "representation"};
    if (format.length() == 0) // ilwis3
        (*this->ptr())->connectTo(QUrl(output), getStoreFormat(), "ilwis3", Ilwis::IlwisObject::ConnectorMode::cmOUTPUT, options.ptr());
    else if ((std::find(ilwis3formats.begin(), ilwis3formats.end(), format) != ilwis3formats.end()) && (fnamespace.length() == 0 || fnamespace == "ilwis3")) // ilwis3
        (*this->ptr())->connectTo(QUrl(output), QString::fromStdString(format), "ilwis3", Ilwis::IlwisObject::ConnectorMode::cmOUTPUT, options.ptr());
    else if (fnamespace.length() == 0) // gdal
        (*this->ptr())->connectTo(QUrl(output), QString::fromStdString(format), "gdal", Ilwis::IlwisObject::ConnectorMode::cmOUTPUT, options.ptr());
    else
        (*this->ptr())->connectTo(QUrl(output), QString::fromStdString(format), QString::fromStdString(fnamespace), Ilwis::IlwisObject::ConnectorMode::cmOUTPUT, options.ptr());
    if (!(*this->ptr())->store(options.ptr()))
        throw OSError(std::string("IOError on attempt to store ")+this->name());
}

bool IlwisObject::__bool__() const{
    if(this->_ilwisObject != NULL && this->_ilwisObject->isValid()){
        if((*this->_ilwisObject)->ilwisType() != itITEMDOMAIN){
            return (*this->_ilwisObject)->isValid();
        } else {
            return true;
        }
    }
    return false;
}

std::string IlwisObject::__str__(){
    if (this->__bool__())
        return (*this->ptr())->name().toStdString();
    else
        return  std::string("invalid IlwisObject!");
}

std::string IlwisObject::name(){
    if (this->__bool__())
        return (*this->ptr())->name().toStdString();
    else
        return  std::string("invalid IlwisObject!");
}

void IlwisObject::name(std::string name){
    (*this->ptr())->name(QString::fromStdString(name));
}

bool IlwisObject::isInternal() const{
    return (*this->ptr())->isInternalObject();
}

std::string IlwisObject::type(){
    if (this->__bool__())
        return Ilwis::IlwisObject::type2Name((*this->ptr())->ilwisType()).toStdString();
    else
        return  std::string("invalid IlwisObject!");
}

std::string IlwisObject::type2Name(IlwisTypes ilwType){
    if (this->__bool__())
        return Ilwis::IlwisObject::type2Name(ilwType).toStdString();
    else
        return  std::string("invalid IlwisObject!");
}

std::string IlwisObject::__add__(std::string value){
    if (this->__bool__())
        return this->__str__()+value;
    else
        return std::string("invalid IlwisObject!");
}

std::string IlwisObject::__radd__(std::string value){
    if (this->__bool__())
        return value+this->__str__();
    else
        return std::string("invalid IlwisObject!");
}

std::shared_ptr<Ilwis::IIlwisObject> IlwisObject::ptr() const{
    if (!this->__bool__())
        throw InvalidObject("invalid IlwisObject");
    return this->_ilwisObject;
}

quint64 IlwisObject::ilwisID() const{
    return (*this->ptr())->id();
}

IlwisTypes IlwisObject::ilwisType(){
    return (*this->ptr())->ilwisType();
}

const QString IlwisObject::getStoreFormat() const {
    return "";
}
