#include <QCoreApplication>
#include <QSettings>
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

Ilwis::IlwisContext *Ilwis::IlwisContext::_context = 0;



using namespace Ilwis;


IlwisContext* Ilwis::context() {
    if (Ilwis::IlwisContext::_context == 0) {
        Ilwis::IlwisContext::_context = new Ilwis::IlwisContext();
        Ilwis::IlwisContext::_context->init();

    }
    return Ilwis::IlwisContext::_context;
}



IlwisContext::IlwisContext() : _workingCatalog(0), _memoryLimit(9e8), _memoryLeft(_memoryLimit)
{
    QDir localDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    _cacheLocation = QUrl::fromLocalFile(localDir.absolutePath());
    QStringList files = localDir.entryList(QStringList() << "*.*", QDir::Files);
    foreach(QString file, files)
        localDir.remove(file);
    _workingCatalog = new Catalog(); // empty catalog>
}

IlwisContext::~IlwisContext()
{
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

void IlwisContext::loadConfigFile(QFileInfo configFile){
    if (configFile.exists()){
        QSettings settings(configFile.filePath(), QSettings::IniFormat);
        this->_ilwisDir = QFileInfo(settings.value("Paths/ilwisDir").toString());
        if (!this->_ilwisDir.isDir()){
            printf("Ilwis directory %s from config file not found\n",this->_ilwisDir.filePath().toStdString().c_str());
            this->_ilwisDir = QFileInfo( qApp->applicationDirPath());
        }
        QString ilwconf =  _ilwisDir.filePath() + "resources/ilwis.config";
        //readJSON(ilwconf);
    }else{
        this->_ilwisDir = QFileInfo( qApp->applicationDirPath());
    }
}

QFileInfo IlwisContext::ilwisFolder() const {
    return this->_ilwisDir;
}

void IlwisContext::init()
{
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
            configfile = qApp->applicationDirPath() + "/resources/ilwis.config";
            file.setFile(configfile);
        }
    }
    _configuration.prepare(file.absoluteFilePath());
    this->_ilwisDir = QFileInfo( qApp->applicationDirPath());
}

ICatalog IlwisContext::workingCatalog() const{
//    if ( _workingCatalog.hasLocalData())
//        return static_cast<Catalog *>(_workingCatalog.localData());

    return _workingCatalog;
}

void IlwisContext::setWorkingCatalog(const ICatalog &cat)
{
    if ( !cat.isValid())
        return;

    mastercatalog()->addContainer(cat->source().url());
    _workingCatalog = cat;
}

QUrl IlwisContext::cacheLocation() const
{
    return _cacheLocation;
}

quint64 IlwisContext::memoryLeft() const
{
    return _memoryLeft;
}
quint64 IlwisContext::changeMemoryLeft(quint64 amount)
{
    if ( _memoryLeft + std::abs(amount) > 0) {
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




