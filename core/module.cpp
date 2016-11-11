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
#include "supportlibraryloader.h"
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

void ModuleMap ::loadPlugin(const QFileInfo& file){
    QPluginLoader loader(file.absoluteFilePath());
    QObject *plugin = loader.instance();
    if (plugin)  {
        Module *module = qobject_cast<Module *>(plugin);
        if (module != 0) {
            module->prepare();
            insert(module->name(),module);

        }
    }
}

void ModuleMap::addModules(const QString& path) {

    QDir folder(path);
    QFileInfoList dirs = folder.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(auto entry : dirs){
        QFileInfo libraryconfig = entry.absoluteFilePath() + "/resources/libraries.config";
        if ( libraryconfig.exists()){
            SupportLibraryLoader libloader(libraryconfig.absoluteFilePath());
            if ( libloader.isValid())
                libloader.loadLibraries();
        }
    }

    for(auto entry : dirs){
        QStringList exts;
        exts << "*.dll" << "*.so" << "*.DLL" << "*.SO";
        QDir plugindir(entry.absoluteFilePath());
        QFileInfoList libs = plugindir.entryInfoList(exts, QDir::Files);
        for( auto lib : libs){
            loadPlugin(lib);
        }
    }
    QString file = context()->ilwisFolder().absoluteFilePath() + "/httpserver.dll";
    loadPlugin(file);
    initModules();

}

void ModuleMap::initModules(){
    foreach(Module *module,*this)
        module->finalizePreparation();

}
