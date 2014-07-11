#include <QDir>
#include <QDirIterator>
#include <QPluginLoader>
#include "module.h"
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"
#include "mastercatalog.h"
#include "ilwisdata.h"
#include "ilwiscontext.h"
#include "errorobject.h"
#include "version.h"



using namespace Ilwis;


//-----------------------------------
Module::Module(QObject *parent, const QString& name, const QString &ivers, const QString& vers) :
    QObject(parent),
    _moduleName(name),
    _interfaceVersion(ivers),
    _version(vers)
{
}

Module::~Module() {
}


QString Module::getInterfaceVersion() const{
    return _interfaceVersion;
}

QString Module::name() const {
    return _moduleName;
}

QString Module::version() const {
    return _version;
}

QString Module::getNameAndVersion() const{
    return _moduleName + " " + _version;
}

void Module::prepare() {
}

void Module::finalizePreparation(){

}

//------------------------------------------------------
ModuleMap::~ModuleMap() {
//    foreach(Module *m, *this){
//        delete m;
//    }
}

void ModuleMap ::loadPlugin(const QString& file){
    QPluginLoader loader(file);
    QObject *plugin = loader.instance();
    if (plugin)  {
        Module *module = qobject_cast<Module *>(plugin);
        if (module != 0) {
            module->prepare();
            insert(module->name(),module);

        }
    }
}

void ModuleMap::addModules() {

    QString path = context()->ilwisFolder().absoluteFilePath() + "/extensions/";
    QDir folder(path);
    QDirIterator dirWalker(folder, QDirIterator::Subdirectories);
    while(dirWalker.hasNext()) {
        dirWalker.next();
        QString file = dirWalker.filePath();

        loadPlugin(file);
    }
    QString file = context()->ilwisFolder().absoluteFilePath() + "/httpserver.dll";
    loadPlugin(file);
    initModules();
}

void ModuleMap::initModules(){
    foreach(Module *module,*this)
        module->finalizePreparation();

}
