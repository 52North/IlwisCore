#include <cmath>
#include "Python.h"
#include <QDir>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "location.h"
#include "workflownode.h"
#include "workflow.h"
#include "commandhandler.h"
#include "operation.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "pythonobjectfactory.h"
#include "connectorfactory.h"
#include "ilwiscontext.h"
#include "runpython.h"
#include "runpythonprogram.h"
#include "pythonworkflowconnector.h"
#include "pythonscriptconnector.h"
#include "pythonscript/pythonscriptmodule.h"
#include "callpythonoperation.h"

using namespace Ilwis;
using namespace PythonScript;

PythonScriptModule::PythonScriptModule(QObject *parent) : Module(parent, "PythonScriptModule", "iv40","1.0")
{

}

PythonScriptModule::~PythonScriptModule()
{
    Py_Finalize();
}

QString PythonScriptModule::getInterfaceVersion() const
{
    return "iv40";
}

QString PythonScriptModule::getName() const
{
    return "pythonscript plugin";
}

QString PythonScriptModule::getVersion() const
{
    return "1.0";
}

void PythonScriptModule::prepare()
{
    try{
        QString home = ilwisconfig("extensionsettings/pythonscript/python-home", QString(sUNDEF));
        if ( home == sUNDEF){
            home = context()->ilwisFolder().absoluteFilePath() + "/extensions/pythonscript/python";
        }
        QString path = qgetenv("PATH");
        path += ";" + home;
        qputenv("PATH", path.toLatin1());

        QString checkhome = home + "/python3.dll";
        QFileInfo inf(checkhome);
        if ( !inf.exists()){
            WARN2(ERR_COULD_NOT_LOAD_2,"Python instance","Python scripting unavailable");
            return;
        }
        wchar_t data[400];
        std::memset(data, 0, 400);
        if ( home.toWCharArray(data) > 0){
            Py_SetPythonHome(data);
            Py_Initialize();
        }else {
            WARN2(ERR_COULD_NOT_LOAD_2,"Python instance","Python scripting unavailable");
            return;
        }
        commandhandler()->addOperation(RunPython::createMetadata(), RunPython::create);
        commandhandler()->addOperation(RunPythonProgram::createMetadata(), RunPythonProgram::create);

        PythonObjectFactory *objfactory = new PythonObjectFactory();
        objfactory->prepare();
        kernel()->addFactory(objfactory );

        ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
        if (!factory)
            return ;

        factory->addCreator(itSCRIPT,"python",PythonScript::PythonScriptConnector::create);

        //factory->addCreator("fileworkflow","python",PythonScript::PythonWorkflowConnector::create);
        factory->addCreator("inmemoryworkflow","python",PythonScript::PythonWorkflowConnector::create2);
        factory->addCreator("script","python",PythonScript::PythonScriptConnector::create);

        quint64 id = mastercatalog()->name2id("ilwis://operations/callpython", itSINGLEOPERATION);
        if ( id != itUNKNOWN){
            loadPythonOperations(id);
        }

        kernel()->issues()->log("Loaded python script module",IssueObject::itMessage);
    } catch(std::exception ex){
        qDebug() << ex.what();
    }



}

void PythonScriptModule::loadPythonOperations(quint64 id){
    QString path = context()->ilwisFolder().absoluteFilePath() + "/extensions/";
    QDir folder(path);
    QFileInfoList dirs = folder.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for(auto entry : dirs){

        QStringList exts;
        exts << "*.py" << "*.PY" << "*.Py" << "*.pY" ;
        QDir pydir(entry.absoluteFilePath());
        if ( pydir.dirName() != "pythonscript"){ // else we would be checking all the py's of local python
            QFileInfoList pyFiles = pydir.entryInfoList(exts, QDir::Files);
            for( auto pyFile : pyFiles){
                loadOperations(pyFile, id);
            }
        }
    }
}

bool PythonScriptModule::splitLine(QString line, QString& front, QString& back, bool toLower){
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

void PythonScriptModule::loadOperations(const QFileInfo& pyFileInfo, quint64 id){
    QFile file(pyFileInfo.absoluteFilePath());
    if (file.open(QFile::ReadOnly)){
        QString pyText = file.readAll();
        int index = -1;
        while((index = pyText.indexOf("#- ILWIS OBJECTS OPERATION METADATA", index + 1)) != -1){
            int index2 = pyText.indexOf("#- END ILWIS OBJECTS OPERATION METADATA", index + 1);
            if ( index2 != -1){
                QString metadata = pyText.mid(index, index2 - index);
                QStringList lines = metadata.split("\n");
                if ( lines.size() > 4){
                    QString front, back;
                    if ( splitLine(lines[1], front, back)){
                        if ( front == "operation"){
                            OperationResource resource(back,QUrl::fromLocalFile(pyFileInfo.absoluteFilePath()));
                            resource.createTime(Time(pyFileInfo.created()));
                            resource.modifiedTime(Time(pyFileInfo.lastModified()));
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
                                CallPythonOperation::addOperation(resource.name(),pyFileInfo.absoluteFilePath());
                                mastercatalog()->addItems({resource});
                            }
                        }else
                            continue;
                    }

                }
            }
        }
    }
}
