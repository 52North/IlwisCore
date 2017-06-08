#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "catalogexplorer.h"
#include "foldercatalogexplorer.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operation.h"
#include "callpythonoperation.h"
#include "pythonscriptcatalogexplorer.h"

using namespace Ilwis;
using namespace PythonScript;

REGISTER_CATALOGEXPLORER(PythonScriptCatalogExplorer)

Ilwis::PythonScript::PythonScriptCatalogExplorer::PythonScriptCatalogExplorer(const Ilwis::Resource &resource, const IOOptions &options) :
    CatalogExplorer(resource, options)
{

}

std::vector<Ilwis::Resource> PythonScriptCatalogExplorer::loadItems(const IOOptions &options)
{
    QStringList sfilters;
    sfilters << "*.py" ;
    std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(source(),
                                                                 sfilters,
                                                                 CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

    kernel()->issues()->silent(true);
    std::vector<Resource> resources;
    quint64 id = mastercatalog()->name2id("ilwis://operations/callpython", itSINGLEOPERATION);
    for(const auto& file : files){
        if(!toLocalFile(file).isDir()){
            QString filepath = file.toLocalFile();
            QFile file(filepath);
            if ( file.open(QIODevice::ReadOnly|QIODevice::Text)){
                QString line = file.readLine();
                if ( line.indexOf("#- ILWIS OBJECTS OPERATION METADATA") == 0){
                    auto operations = loadOperationResource(file,filepath,id) ;
                    std::copy(operations.begin(), operations.end(), std::back_inserter(resources));
                }else{
                    Resource res(filepath, itSCRIPT);
                    resources.push_back(res);
                }
                file.close();
            }

        }
    }

    kernel()->issues()->log(QString(TR("Added %1 objects through the script connector")).arg( resources.size()),IssueObject::itMessage);

    kernel()->issues()->silent(false);

    return resources;
}

bool PythonScriptCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if ( resource.url().scheme() != "file") // can't use anything marked as file
        return false;
    if (resource.url().scheme() == "ilwis")
        return false;
    if ( QFileInfo(resource.url(true).toLocalFile()).isFile()) // no file can be an py folder catalog.
        return false;
    return true;

}

QString PythonScriptCatalogExplorer::provider() const
{
    return "python";
}

QFileInfo PythonScriptCatalogExplorer::toLocalFile(const QUrl &datasource) const
{
    return QFileInfo(datasource.toLocalFile());
}

Ilwis::CatalogExplorer *PythonScriptCatalogExplorer::create(const Resource &resource, const IOOptions &options)
{
    return new PythonScriptCatalogExplorer(resource, options) ;
}

bool PythonScriptCatalogExplorer::splitLine(QString line, QString& front, QString& back, bool toLower){
    line.remove("#-");
    line = line.trimmed();
    if ( toLower)
        line.toLower();
    QStringList parts = line.toLower().split("=");
    if ( parts.size() != 2)
        return false;
    front = parts[0].trimmed().toLower();
    back = parts[1].trimmed();

    return true;
}

std::vector<Resource> PythonScriptCatalogExplorer::loadOperationResource(QFile& file, const QString& filename, quint64 id){

    file.seek(0);
    QString pyText = file.readAll();
    int index = -1;
    std::vector<Resource> result;
    while((index = pyText.indexOf("#- ILWIS OBJECTS OPERATION METADATA", index + 1)) != -1){
        int index2 = pyText.indexOf("#- END ILWIS OBJECTS OPERATION METADATA", index + 1);
        if ( index2 != -1){
            QString metadata = pyText.mid(index, index2 - index);
            QStringList lines = metadata.split("\n");
            if ( lines.size() > 4){
                QString front, back;
                if ( splitLine(lines[1], front, back)){
                    if ( front == "operation"){
                        OperationResource resource(back,QUrl::fromLocalFile(filename));
                        resource.setExtendedType(itSCRIPT);
                         QFileInfo inf(filename);
                        resource.createTime(Time(inf.created()));
                        resource.modifiedTime(Time(inf.lastModified()));
                        int  i = 2;
                        while ( i < lines.size() && splitLine(lines[i], front, back)){
                            if ( front == "longname")
                                resource.setLongName(back);
                            else if ( front == "syntax")
                                resource.setSyntax(back);
                            else if ( front == "description")
                                resource.setDescription(back);
                            else if ( front == "input parameter count" ){
                                QStringList counts = back.split("|");
                                std::vector<quint32> numbers;
                                for(auto v : counts)
                                    numbers.push_back(v.toInt());
                                resource.setInParameterCount(numbers);
                            }
                            else if ( front == "output parameter count" ){
                                QStringList counts = back.split("|");
                                std::vector<quint32> numbers;
                                for(auto v : counts)
                                    numbers.push_back(v.toInt());
                                resource.setOutParameterCount(numbers);
                            }
                            else if ( front.indexOf("input parameter") == 0) {
                                QStringList forder = front.split(" ");
                                if ( forder.size() != 3){
                                    i = 1000;
                                    continue;
                                }
                                int ord = forder[2].toInt() - 1;
                                QStringList data = back.split("|");
                                QString label=sUNDEF, description=sUNDEF;
                                IlwisTypes tp = TypeHelper::name2type(data[0]);
                                if ( tp == itUNKNOWN)
                                    i = 1000;
                                else {
                                    if ( data.size() > 1)
                                        label = data[1];
                                    if ( data.size() == 3)
                                        description = data[2];
                                    resource.addInParameter(ord,tp,label,description);

                                }
                            }
                            else if ( front.indexOf("output parameter") == 0){
                                QStringList forder = front.split(" ");
                                if ( forder.size() != 3){
                                    i = 1000;
                                    continue;
                                }
                                int ord = forder[2].toInt() - 1;
                                QStringList data = back.split("|");
                                QString label=sUNDEF, description=sUNDEF;
                                IlwisTypes tp = TypeHelper::name2type(data[0]);
                                if ( tp == itUNKNOWN)
                                    i = 1000;
                                else {
                                    if ( data.size() > 1)
                                        label = data[1];
                                    if ( data.size() == 3)
                                        description = data[2];
                                    resource.addOutParameter(ord,tp,label,description);

                                }
                            }
                            else if ( front == "keywords"){
                                back.replace("|",",");
                                resource.setKeywords(back);
                            }
                            ++i;

                        }
                        if ( i <= 1000){
                            resource.addProperty("stuboperation", id);
                            CallPythonOperation::addOperation(resource.name(),filename);
                            result.push_back(resource);

                        }
                    }else
                        continue;
                }

            }
        }
    }
    return result;
}

