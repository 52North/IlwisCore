#include <QCoreApplication>
#include <QSettings>
#include <QDir>
#include <QDesktopServices>
#include <QJsonDocument>
#include "kernel.h"
#include "ilwisdata.h"
#include "errorobject.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "containerconnector.h"
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
    QStringList files = localDir.entryList(QStringList() << "*.*", QDir::Files);
    foreach(QString file, files)
        localDir.remove(file);
    _workingCatalog = new Catalog(); // empty catalog>
}

IlwisContext::~IlwisContext()
{
    delete _workingCatalog;
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
        this->_ilwisDir = QFileInfo(settings.value("ilwisDir").toString());
        if (!this->_ilwisDir.isDir()){
            printf_s("Ilwis directory %s from config file not found\n",this->_ilwisDir.filePath().toStdString().c_str());
            this->_ilwisDir = QFileInfo( qApp->applicationDirPath());
        }
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
    QFile file;
    file.setFileName(configfile);
    if (file.open(QIODevice::ReadOnly)) {
        QString settings = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(settings.toUtf8());
        if ( !doc.isNull())
            _configuration = doc.object();
    }
    this->_ilwisDir = QFileInfo( qApp->applicationDirPath());
}

Catalog *IlwisContext::workingCatalog() const{
//    if ( _workingCatalog.hasLocalData())
//        return static_cast<Catalog *>(_workingCatalog.localData());

    return _workingCatalog;
}

void IlwisContext::setWorkingCatalog(const Catalog &cat)
{
    if ( _workingCatalog) {
        delete _workingCatalog;
    }
    mastercatalog()->addContainer(cat.location());
    _workingCatalog = new Catalog(cat);
}

QUrl IlwisContext::temporaryWorkLocation() const
{
    QString loc = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    return QUrl::fromLocalFile(loc);
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

QJsonObject IlwisContext::configuration() const
{
    return _configuration;
}


