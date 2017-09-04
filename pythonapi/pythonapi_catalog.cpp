#include "../../core/identity.h"
#include "../../core/kernel.h"
#include "../../core/ilwiscontext.h"
#include "../../core/catalog/catalog.h"
#include "../../core/ilwisobjects/ilwisdata.h"
#include "../../core/catalog/resource.h"
#include "../../core/catalog/mastercatalog.h"

#include "pythonapi_catalog.h"

#include "pythonapi_pyobject.h"
#include "pythonapi_error.h"

#include "../../core/ilwisobjects/coverage/coverage.h"
#include "../../core/ilwisobjects/domain/numericrange.h"
#include "../../core/ilwisobjects/domain/numericdomain.h"
#include "../../core/ilwisobjects/table/columndefinition.h"
#include "../../core/ilwisobjects/table/table.h"
#include "../../core/ilwisobjects/table/attributedefinition.h"
#include "../../core/ilwisobjects/coverage/featurecoverage.h"
#include "../../core/ilwisobjects/coverage/feature.h"
#include "../../core/factory.h"
#include "../../core/abstractfactory.h"
#include "../../core/ilwisobjects/coverage/featurefactory.h"

#include "pythonapi_featurecoverage.h"

#include "../../core/ilwisobjects/coverage/raster.h"
#include "../../core/ilwisobjects/coverage/rastercoverage.h"
#include "pythonapi_rastercoverage.h"

#include "pythonapi_domain.h"

#include "../../core/ilwisobjects/geometry/coordinatesystem/projection.h"
#include "../../core/ilwisobjects/geometry/coordinatesystem/ellipsoid.h"
#include "../../core/ilwisobjects/operation/operationmetadata.h"

namespace pythonapi {

    Catalog::Catalog(const std::string& url, const std::string& filter){
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
            else { // pos < 0: file without path, or new object
                QString file = Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
                if (QFileInfo (file).exists()) // file without path
                    input = "file:///" + file;
            }
        }
        QString loc = input; // see MasterCatalog::addContainer(); url must match the _container that gets into the mastercatalog
        if (loc.size() > 2 && loc[loc.size() - 1] == '/' && loc[loc.size() - 2] != '/')
            loc = loc.left(loc.size() - 1);
        QUrl location (loc);
        Ilwis::mastercatalog()->addContainer(location); // this will do the actual scan of "location" and add the sub-items to the mastercatalog
        _catalog.prepare(location.toString());
    }

    Catalog::~Catalog(){
    }

    bool Catalog::__bool__() const{
        return this->_catalog.isValid() && this->_catalog->isValid();
    }

    std::string Catalog::__str__(){
        if (this->__bool__())
            return "Catalog";
        else
            return "invalid Catalog";
    }

    IlwisTypes Catalog::ilwisType(){
        return itCATALOG;
    }

    PyObject* Catalog::items(){
        if (this->__bool__()){
            std::vector<Ilwis::Resource> itms = this->_catalog->items();
            PyObject* ret = newPyTuple(itms.size());
            int i = 0;
            for(auto it = itms.begin(); it < itms.end(); it++ ){
                if (!setTupleItem(ret, i, PyUnicodeFromString(it->name().toStdString().data()))){
                    throw InvalidObject("invalid recource in catalog items");
                }
                i++;
            }
            return ret;
        }else{
            return newPyTuple(0);
        }
    }

    Object* Catalog::_getitem(const std::string &name){
        std::vector<Ilwis::Resource> itms = this->_catalog->items();
        for(auto it = itms.begin(); it < itms.end(); it++ ){
            if(it->name().compare(QString::fromStdString(name), Qt::CaseInsensitive) == 0){
                IlwisTypes type = it->ilwisType();
                if (hasType(type,itFEATURE)){
                    return new FeatureCoverage(Ilwis::IFeatureCoverage(*it));
                }else if (hasType(type,itRASTER)){
                    return new RasterCoverage(Ilwis::IRasterCoverage(*it));
                }else if (hasType(type,itTABLE)){
                    return new Table(Ilwis::ITable(*it));
                }else if (hasType(type,itNUMERICDOMAIN)){
                    return new NumericDomain(Ilwis::INumericDomain(*it));
                }else if (hasType(type,itCOORDSYSTEM)){
                    return new CoordinateSystem(Ilwis::ICoordinateSystem(*it));
                }else if (hasType(type,itGEOREF)){
                    return new GeoReference(Ilwis::IGeoReference(*it));
//                }else if (hasType(type,itOPERATIONMETADATA)){
//                    return new OperationMetaData(Ilwis::IOperationMetaData(*it));
//                }else if (hasType(type,itPROJECTION)){
//                    return new Projection(Ilwis::IProjection(*it));
//                }else if (hasType(type,itELLIPSOID)){
//                    return new Ellipsoid(Ilwis::IEllipsoid(*it));
                }else if (hasType(type,itCATALOG)){

                    Ilwis::mastercatalog()->addContainer(it->url()); // this will do the actual scan of "location" and add the sub-items to the mastercatalog
                    Ilwis::ICatalog cat (it->url().toString());
                    return new Catalog(cat);
                }else{
                    return new IlwisObject(new Ilwis::IIlwisObject(*it));
                }
            }
        }
        throw std::out_of_range(std::string("item not found: ")+name);
    }

    Catalog *Catalog::toCatalog(Object *obj){
        Catalog* ptr = dynamic_cast<Catalog*>(obj);
        if(!ptr)
            throw InvalidObject("cast to Catalog not possible");
        return ptr;
    }

    Catalog::Catalog(Ilwis::ICatalog & cat){
        _catalog = cat;
    }

} // namespace pythonapi
