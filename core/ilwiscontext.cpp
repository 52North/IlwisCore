#include <QCoreApplication>
#include <QSettings>
#include <QDir>
#include <QDesktopServices>
#include "kernel.h"
#include "ilwisdata.h"
#include "errorobject.h"
#include "factory.h"
#include "abstractfactory.h"
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
}

IlwisContext::~IlwisContext()
{
    delete _workingCatalog;
}

void IlwisContext::addSystemLocation(const QUrl &res)
{
    if ( _systemlocations.indexOf(res) < 0) {
        _systemlocations.push_back(res);
    }

}

void IlwisContext::removeSystemLocation(const QUrl &)
{
    //TODO
}

QFileInfo IlwisContext::ilwisFolder() const {
    return QFileInfo( qApp->applicationDirPath());
}

void IlwisContext::init()
{
    QSettings settings("52n","ilwis4");

    QString working = settings.value("workingcatalog",QVariant(sUNDEF)).toString();
    if ( working == sUNDEF) {

    }
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


