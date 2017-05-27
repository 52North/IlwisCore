#include <QCoreApplication>
#include <QDir>
#include <QDesktopServices>
#include <QJsonDocument>
#include "kernel.h"
#include "ilwisdata.h"
#include "errorobject.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "mastercatalog.h"
#include "mastercatalogcache.h"

Ilwis::IlwisContext *Ilwis::IlwisContext::_context = 0;

using namespace Ilwis;

IlwisContext* Ilwis::context(const QString & ilwisDir, int runMode) {
    if (Ilwis::IlwisContext::_context == 0) {
        Ilwis::IlwisContext::_context = new Ilwis::IlwisContext(runMode);
        Ilwis::IlwisContext::_context->init(ilwisDir);

    }
    return Ilwis::IlwisContext::_context;
}



IlwisContext::IlwisContext(int runMode) : _workingCatalog(0), _memoryLimit(9e8), _memoryLeft(_memoryLimit), _runMode(runMode)
{
    // _workingCatalog = new Catalog(); // empty catalog>

}

IlwisContext::~IlwisContext()
{
    if ( _workingCatalog.isValid()){
        _configuration.putValue("users/" + currentUser() + "/workingcatalog",_workingCatalog->resource().url().toString());
        _configuration.store();
    }
}

void IlwisContext::addSystemLocation(const QUrl &resource)
{
    if ( std::find(_systemlocations.begin(), _systemlocations.end(), resource) == _systemlocations.end() ) {
        _systemlocations.push_back(resource);
    }

}

void IlwisContext::removeSystemLocation(const QUrl &)
{
    //TODO:
}

QFileInfo IlwisContext::ilwisFolder() const {
    return this->_ilwisDir;
}

void IlwisContext::init(const QString &ilwisDir)
{
    if (ilwisDir.length() > 0) {
        this->_ilwisDir = QFileInfo(ilwisDir);
        if (!this->_ilwisDir.isDir()) {
            printf("User-supplied Ilwis directory '%s' not found\n",this->_ilwisDir.filePath().toStdString().c_str());
            this->_ilwisDir = QFileInfo(qApp->applicationDirPath());
        } else
            qApp->addLibraryPath(this->_ilwisDir.absolutePath() + "/plugins"); // also inform Qt where its "plugins" folder is installed, so that it doesn't use the hardcoded qt_plugpath inside Qt5Core.dll
    } else
        this->_ilwisDir = QFileInfo(qApp->applicationDirPath());

    QString loc = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

    QString configfile = loc + "/" + "ilwis.config";
    QFileInfo file;
    file.setFile(configfile);
    if ( !file.exists()){
        if ( _ilwisDir.exists()){
            configfile = _ilwisDir.absoluteFilePath() + "/resources/ilwis.config";
            file.setFile(configfile);
            if (!file.exists()){
                configfile = _ilwisDir.absoluteFilePath() + "/ilwis.config";
                file.setFile(configfile);
            }
        } else{

            configfile = _ilwisDir.absoluteFilePath() + "/resources/ilwis.config";
            file.setFile(configfile);
        }
    }
    _configuration.prepare(file.absoluteFilePath());

    QString location = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/cache-location",QString(sUNDEF));
    if ( location == sUNDEF){
        QDir localDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
        _cacheLocation = QUrl::fromLocalFile(localDir.absolutePath());
    }else
        _cacheLocation = QUrl(location);

    QDir cacheDir(_cacheLocation.toLocalFile());

    if (!cacheDir.exists()){
        cacheDir.mkpath(location);
    }
    QStringList files = cacheDir.entryList(QStringList() << "gridblock*.*", QDir::Files);
    for(QString file : files)
        cacheDir.remove(file);

    QString datalocation = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/internalcatalog-location",QString(sUNDEF));
    if ( datalocation == sUNDEF){
        datalocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/internalcatalog";
    }else
        datalocation = QUrl(datalocation).toLocalFile();
    datalocation = OSHelper::neutralizeFileName(datalocation);

    QDir localDir(datalocation);

    if (!localDir.exists()){
        localDir.mkpath(datalocation);
    }
    _persistentInternalCatalog = QUrl::fromLocalFile(datalocation);
    files = localDir.entryList(QStringList() << "*", QDir::Files);
    for(QString file : files)
        localDir.remove(file);



    mastercatalog()->addContainer(INTERNAL_CATALOG_URL);
    Resource res = mastercatalog()->name2Resource(INTERNAL_CATALOG_URL.toString(),itCATALOG);
    res.name("temporary catalog",false,true);
    mastercatalog()->addContainer(persistentInternalCatalog());
    mastercatalog()->addContainer(QUrl("ilwis://operations"));

    _systemCatalog.prepare("ilwis://system");
    mastercatalog()->addContainer(QUrl("ilwis://system/domains"));
    mastercatalog()->addContainer(QUrl("ilwis://system/coordinatesystems"));
    mastercatalog()->addContainer(QUrl("ilwis://system/representations"));
    mastercatalog()->addContainer(QUrl("ilwis://system/ellipsoids"));
    mastercatalog()->addContainer(QUrl("ilwis://system/projections"));
    mastercatalog()->addContainer(QUrl("ilwis://system/datums"));
    mastercatalog()->addContainer(QUrl("ilwis://system/coverages"));
    mastercatalog()->addContainer(QUrl("ilwis://system/scripts"));

    loc = _configuration("users/" + currentUser() + "/workingcatalog",QString(""));
    if ( loc == ""){
        loc = OSHelper::neutralizeFileName(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +  + "/ilwisdata");
        QDir datadir(loc);
        datadir.mkpath(loc);
        loc = QUrl::fromLocalFile(loc).toString();

    }
    if ( loc != ""){
        setWorkingCatalog(ICatalog(loc));
        if ( hasType(_runMode, rmCOMMANDLINE)){
            mastercatalog()->addContainer(loc);
        }
    }

    if (!hasType(_runMode, rmDESKTOP)){
        initializationFinished(true);
    }

}

ICatalog IlwisContext::workingCatalog() const{
//    if ( _workingCatalog.hasLocalData())
//        return static_cast<Catalog *>(_workingCatalog.localData());
    Locker<std::mutex> lock(_lock);
    const QVariant *var = kernel()->getFromTLS("workingcatalog");
    if ( var && var->isValid()){
        ICatalog cat = var->value<ICatalog>();
        return cat;
    }
    return ICatalog();
}

const ICatalog &IlwisContext::systemCatalog() const
{
    return _systemCatalog;
}

void IlwisContext::setWorkingCatalog(const ICatalog &cat)
{
    // the ilwis default workspace is just is a placeholder for everything goes; so we don't assign it
    if ( !cat.isValid() || cat->resource().url().toString() == Catalog::DEFAULT_WORKSPACE)
        return;

    Locker<std::mutex> lock(_lock);
    QVariant *var = new QVariant();
    var->setValue(cat);
    kernel()->setTLS("workingcatalog", var);
    context()->configurationRef().putValue("users/" + currentUser() + "/workingcatalog",cat->resource().url().toString());
    QFileInfo inf(cat->resource().url().toLocalFile());
}

QUrl IlwisContext::cacheLocation() const
{
    return _cacheLocation;
}

QUrl IlwisContext::persistentInternalCatalog() const
{
    return _persistentInternalCatalog;
}

quint64 IlwisContext::memoryLeft() const
{
    return _memoryLeft;
}
quint64 IlwisContext::changeMemoryLeft(qint64 amount)
{
    if ( (_memoryLeft + amount) > 0) {
        _memoryLeft += amount;
    }
    else
        _memoryLeft = 0;

    return _memoryLeft;
}

IlwisConfiguration &IlwisContext::configurationRef()
{
    return _configuration;
}

const IlwisConfiguration &IlwisContext::configuration() const
{
    return _configuration;
}

QFileInfo IlwisContext::resourceRoot() const
{
    QString root = ilwisconfig("system-settings/resource-root",QString("app-base"));
    if ( root == "app-base"){
        return QFileInfo(ilwisFolder().absoluteFilePath() + "/resources");
    }
    QFileInfo inf(root + "/resources");
    if ( inf.exists())
        return inf;

    return QFileInfo(ilwisFolder().absoluteFilePath() + "/resources");
}

QString IlwisContext::ipv4() const
{
    if ( _ipv4 == sUNDEF){
        const_cast<IlwisContext *>(this)->_ipv4 = const_cast<IlwisContext *>(this)->configurationRef()("server-settings/ipv4-address", QString(""));
    }
    return _ipv4;
}

QString IlwisContext::currentUser() const
{
    return "user-0"; // by default atm,.
}

int IlwisContext::runMode() const
{
    return _runMode;
}

void IlwisContext::runMode(int mode)
{
    _runMode = mode;
}

bool IlwisContext::initializationFinished() const
{
   // Locker<stdV::mutex> locker(_lock);
    return _initializationFinished;
}

void IlwisContext::initializationFinished(bool yesno)
{
   //Locker<std::mutex> locker(_lock);
    _initializationFinished = yesno;
}





