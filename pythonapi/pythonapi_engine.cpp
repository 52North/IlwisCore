#include "../../core/kernel.h"
#include "../../core/ilwiscontext.h"
#include "../../core/catalog/catalog.h"

#include "../../core/ilwisobjects/ilwisdata.h"
#include "../../core/ilwisobjects/operation/operationmetadata.h"
#include "../../core/ilwisobjects/operation/symboltable.h"
#include "../../core/ilwisobjects/operation/commandhandler.h"
#include "../../core/ilwisobjects/operation/operationExpression.h"
#include "../../core/ilwisobjects/operation/operation.h"

#include "../../core/ilwisobjects/ilwisobject.h"

#include "../../core/ilwisobjects/domain/domain.h"
#include "../../core/ilwisobjects/domain/datadefinition.h"
#include "../../core/ilwisobjects/table/columndefinition.h"
#include "../../core/ilwisobjects/table/table.h"
#include "../../core/ilwisobjects/table/attributedefinition.h"

#include "../../core/ilwisobjects/coverage/raster.h"
#include "../../core/ilwisobjects/coverage/coverage.h"

#include "../../core/ilwisobjects/coverage/rastercoverage.h"

#include "../../core/util/ilwisangle.h"

#include "../../core/util/box.h"

#include "../../core/ilwisobjects/coverage/featurecoverage.h"
#include "../../core/ilwisobjects/coverage/feature.h"


#include "pythonapi_object.h"
#include "pythonapi_engine.h"
#include "pythonapi_collection.h"
#include "pythonapi_rastercoverage.h"
#include "pythonapi_featurecoverage.h"
#include "pythonapi_pyobject.h"
#include "pythonapi_catalog.h"
#include "pythonapi_table.h"
#include "pythonapi_booleanobject.h"

using namespace pythonapi;

Engine::Engine(){
}

qint64 Engine::_do2(std::string output_name, std::string operation, std::string c3, std::string c4, std::string c5,std::string c6, std::string c7, std::string c8, std::string c9){
    Ilwis::SymbolTable symtbl;
    Ilwis::ExecutionContext ctx;
    ctx.clear();
    //is no internal result name is given it will look like operation_id
    //but the id is to be added afterwards
    bool rename = false;
    if (output_name.empty()){
        output_name = operation;
        rename = true;
    }
    QString command;
    if (!c3.empty()){
        if(!c4.empty()){
            if(!c5.empty()){
                if(!c6.empty()){
                    if(!c7.empty()){
                        if(!c8.empty()){
                            if(!c9.empty()){
                                command = QString("script %1=%2(%3,%4,%5,%6,%7,%8,%9)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str(),c7.c_str(),c8.c_str(),c9.c_str());
                            }else{
                                command = QString("script %1=%2(%3,%4,%5,%6,%7,%8)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str(),c7.c_str(),c8.c_str());
                            }
                        }else{
                            command = QString("script %1=%2(%3,%4,%5,%6,%7)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str(),c7.c_str());
                        }
                    }else{
                        command = QString("script %1=%2(%3,%4,%5,%6)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str());
                    }
                }else{
                    command = QString("script %1=%2(%3,%4,%5)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str());
                }
            }else{
                command = QString("script %1=%2(%3,%4)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str());
            }
        }else{
            command = QString("script %1=%2(%3)").arg(output_name.c_str(),operation.c_str(),c3.c_str());
        }
    }else{
        command = QString("script %1=%2").arg(output_name.c_str(),operation.c_str());
    }
    if (Ilwis::commandhandler()->execute(command,&ctx, symtbl) && !ctx._results.empty()){
        //std::vector<Object*> results;
        for (int i = 0; i < ctx._results.size(); ++i) {
            Ilwis::Symbol result = symtbl.getSymbol(ctx._results[i]);
            if (result._type == itRASTER){
                if (result._var.canConvert<Ilwis::IRasterCoverage>()){
                    Ilwis::IRasterCoverage obj (result._var.value<Ilwis::IRasterCoverage>());
                    return obj->id();
                }
            }else if (result._type == itFEATURE){
                if (result._var.canConvert<Ilwis::IFeatureCoverage>()){
                    Ilwis::IFeatureCoverage obj (result._var.value<Ilwis::IFeatureCoverage>());
                    return obj->id();
                }
            }else if (result._type == itCOORDSYSTEM){
                if (result._var.canConvert<Ilwis::ICoordinateSystem>()){
                    Ilwis::ICoordinateSystem obj (result._var.value<Ilwis::ICoordinateSystem>());
                    return obj->id();
                }
            }else if (result._type == itGEOREF){
                if (result._var.canConvert<Ilwis::IGeoReference>()){
                    Ilwis::IGeoReference obj (result._var.value<Ilwis::IGeoReference>());
                    return obj->id();
                }
            }else if (result._type & itTABLE){
                if (result._var.canConvert<Ilwis::ITable>()){
                    Ilwis::ITable obj (result._var.value<Ilwis::ITable>());
                    return obj->id();
                }
            } else {
                auto list = result._var.toList();
                if ( list.size() > 0)
                    return list[0].toDouble();
                return rUNDEF;
            }
        }
        /*
        if (results.size() == 0)
            throw Ilwis::ErrorObject(QString("couldn't handle return type of \"%1\"").arg(command.mid(8 + output_name.size())));
        else if (results.size() == 1)
            return results[0];
        else {
            return new Collection(results);
        }
        */
    }else{
        QString filter = QString("(type=%1 or type=%2)").arg(itSINGLEOPERATION).arg(itWORKFLOW);
        std::vector<Ilwis::Resource> ops = Ilwis::mastercatalog()->select(filter);
        bool found = false;
        for(auto it = ops.begin(); it != ops.end(); it++){
            if (it->name().toStdString() == operation) {
                found = true;
                break;
            }
        }
        if (found)
            throw Ilwis::ErrorObject(QString("Failed to execute command \"%1\"; Please check the parameters provided.").arg(command.mid(8 + output_name.size())));
        else
            throw Ilwis::ErrorObject(QString("Command \"%1\" does not exist; See ilwis.Engine.operations() for the full list.").arg(operation.c_str()));
    }
}

Object* Engine::_do(std::string output_name, std::string operation, std::string c3, std::string c4, std::string c5,std::string c6, std::string c7, std::string c8, std::string c9){
    Ilwis::SymbolTable symtbl;
    Ilwis::ExecutionContext ctx;
    ctx.clear();
    //is no internal result name is given it will look like operation_id
    //but the id is to be added afterwards
    bool rename = false;
    if (output_name.empty()){
        output_name = operation;
        rename = true;
    }
    QString command;
    if (!c3.empty()){
        if(!c4.empty()){
            if(!c5.empty()){
                if(!c6.empty()){
                    if(!c7.empty()){
                        if(!c8.empty()){
                            if(!c9.empty()){
                                command = QString("script %1=%2(%3,%4,%5,%6,%7,%8,%9)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str(),c7.c_str(),c8.c_str(),c9.c_str());
                            }else{
                                command = QString("script %1=%2(%3,%4,%5,%6,%7,%8)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str(),c7.c_str(),c8.c_str());
                            }
                        }else{
                            command = QString("script %1=%2(%3,%4,%5,%6,%7)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str(),c7.c_str());
                        }
                    }else{
                        command = QString("script %1=%2(%3,%4,%5,%6)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str(),c6.c_str());
                    }
                }else{
                    command = QString("script %1=%2(%3,%4,%5)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str(),c5.c_str());
                }
            }else{
                command = QString("script %1=%2(%3,%4)").arg(output_name.c_str(),operation.c_str(),c3.c_str(),c4.c_str());
            }
        }else{
            command = QString("script %1=%2(%3)").arg(output_name.c_str(),operation.c_str(),c3.c_str());
        }
    }else{
        command = QString("script %1=%2").arg(output_name.c_str(),operation.c_str());
    }
    if (Ilwis::commandhandler()->execute(command,&ctx, symtbl)){
        std::vector<Object*> results;
        for (int i = 0; i < ctx._results.size(); ++i) {
            Ilwis::Symbol result = symtbl.getSymbol(ctx._results[i]);
            if (result._type == itRASTER){
                if (result._var.canConvert<Ilwis::IRasterCoverage>()){
                    Ilwis::IRasterCoverage obj (result._var.value<Ilwis::IRasterCoverage>());
                    if (rename)
                        obj->name(QString("%1_%2").arg(operation.c_str()).arg(obj->id()));
                    results.push_back(new RasterCoverage(obj));
                }
            }else if (result._type == itFEATURE){
                if (result._var.canConvert<Ilwis::IFeatureCoverage>()){
                    Ilwis::IFeatureCoverage obj (result._var.value<Ilwis::IFeatureCoverage>());
                    if (rename)
                        obj->name(QString("%1_%2").arg(operation.c_str()).arg(obj->id()));
                    results.push_back(new FeatureCoverage(obj));
                }
            }else if (result._type == itCOORDSYSTEM){
                if (result._var.canConvert<Ilwis::ICoordinateSystem>()){
                    Ilwis::ICoordinateSystem obj (result._var.value<Ilwis::ICoordinateSystem>());
                    if (rename)
                        obj->name(QString("%1_%2").arg(operation.c_str()).arg(obj->id()));
                    results.push_back(new CoordinateSystem(obj));
                }
            }else if (result._type == itGEOREF){
                if (result._var.canConvert<Ilwis::IGeoReference>()){
                    Ilwis::IGeoReference obj (result._var.value<Ilwis::IGeoReference>());
                    if (rename)
                        obj->name(QString("%1_%2").arg(operation.c_str()).arg(obj->id()));
                    results.push_back(new GeoReference(obj));
                }
            }else if (result._type & itTABLE){
                if (result._var.canConvert<Ilwis::ITable>()){
                    Ilwis::ITable obj (result._var.value<Ilwis::ITable>());
                    if (rename)
                        obj->name(QString("%1_%2").arg(operation.c_str()).arg(obj->id()));
                    results.push_back(new Table(obj));
                }
            } else if (result._type == itBOOL){

            }
        }
        if (results.size() == 0)
            return new BooleanObject();
        else if (results.size() == 1)
            return results[0];
        else {
            return new Collection(results);
        }
    }else{
        QString filter = QString("(type=%1 or type=%2)").arg(itSINGLEOPERATION).arg(itWORKFLOW);
        std::vector<Ilwis::Resource> ops = Ilwis::mastercatalog()->select(filter);
        bool found = false;
        for(auto it = ops.begin(); it != ops.end(); it++){
            if (it->name().toStdString() == operation) {
                found = true;
                break;
            }
        }
        if (found)
            throw Ilwis::ErrorObject(QString("Failed to execute command \"%1\"; Please check the parameters provided.").arg(command.mid(8 + output_name.size())));
        else
            throw Ilwis::ErrorObject(QString("Command \"%1\" does not exist; See ilwis.Engine.operations() for the full list.").arg(operation.c_str()));
    }
}

void Engine::setWorkingCatalog(const std::string& location) {
    QString loc (QString::fromStdString(location));
    loc.replace('\\','/');
    // if it is file:// (or http:// etc) leave it untouched; if not, append file:// and the working catalog path if it is missing
    if (loc.indexOf("://") < 0) {
        int pos = loc.indexOf('/');
        if (pos > 0) { // full path starting with drive-letter (MS-DOS-style)
            loc = "file:///" + loc;
            if (loc.endsWith('/')) // workaround an IlwisObjects problem that scans the folder twice if it ends with a slash
                loc = loc.left(loc.length() - 1);
        } else if (pos == 0) { // full path starting with path-separator (UNIX-style)
            loc = "file://" + loc;
            if (loc.endsWith('/'))
                loc = loc.left(loc.length() - 1);
        }
    }

    Ilwis::ICatalog cat;
    cat.prepare(loc);
    if(cat.isValid()){
        Ilwis::context()->setWorkingCatalog(cat);
        Ilwis::mastercatalog()->addContainer(QUrl(loc));
    }else
        throw Ilwis::ErrorObject(QString("invalid container location: '%1'").arg(location.c_str()));
}

std::string Engine::getLocation(){
    Ilwis::ICatalog cat = Ilwis::context()->workingCatalog();
    QUrl location = cat->filesystemLocation();
    return location.toString().toStdString();
}

PyObject* Engine::operations(){
    QString filter = QString("(type=%1 or type=%2)").arg(itSINGLEOPERATION).arg(itWORKFLOW);
    std::vector<Ilwis::Resource> ops = Ilwis::mastercatalog()->select(filter);
    PyObject* list = newPyTuple(ops.size());
    int i = 0;
    for(auto it = ops.begin(); it != ops.end(); it++){
        if (!setTupleItem(list, i++, PyUnicodeFromString(it->name().toStdString().data()))){
            throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of attributes").arg( it->name()));
        }
    }
    return list;
}

std::string Engine::operationMetaData(const std::string &name, const std::string &element){
    QString filter = QString("(type=%1 or type=%2)").arg(itSINGLEOPERATION).arg(itWORKFLOW);
    std::vector<Ilwis::Resource> ops = Ilwis::mastercatalog()->select(filter);
    QString ret;
    for(auto it = ops.begin();it != ops.end(); it++){
        if (QString::fromStdString(name).compare(it->name(),Qt::CaseInsensitive) == 0){
            if(!ret.isEmpty())
                ret.append("; ");
            if (element.compare("description") == 0)
                ret.append(it->description());
            else
                ret.append((*it)[element.c_str()].toString());
        }
    }
    return ret.toStdString();
}

PyObject* Engine::_catalogItems(){
    Ilwis::ICatalog cat = Ilwis::context()->workingCatalog();
    std::vector<Ilwis::Resource> resVec = cat->items();
    PyObject* tup = newPyTuple(resVec.size());
    int i = 0;
    for(auto it = resVec.begin();it != resVec.end(); it++){
        if (!setTupleItem(tup, i++, PyUnicodeFromString(it->name().toStdString().data()))){
            throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of files").arg( it->name()));
        }
    }
    return tup;
}
