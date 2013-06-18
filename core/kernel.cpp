#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCache>
#include <QVector>
#include <QStringList>
#include <QSqlRecord>
#include <QUrl>
#include <QDir>
#include <cxxabi.h>
#include <QException>
#include "kernel.h"
#include "factory.h"
#include "geometries.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "ilwiscontext.h"
#include "catalogconnectorfactory.h"
#include "connectorfactory.h"
#include "catalogconnector.h"
#include "featurefactory.h"
#include "catalog.h"
#include "module.h"
#include "mastercatalog.h"
#include "version.h"
#include "errorobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "range.h"
#include "itemrange.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "geometry.h"
#include "feature.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "operationExpression.h"
#include "operation.h"
#include "commandhandler.h"

Ilwis::Kernel *Ilwis::Kernel::_kernel = 0;

using namespace Ilwis;

Catalog *createCatalog()  {
    return new Catalog();
}

Ilwis::Kernel* kernel() {
    if (Kernel::_kernel == 0) {
        Kernel::_kernel = new Kernel();
        Kernel::_kernel->init();

    }
    return Kernel::_kernel;
}


Kernel::Kernel(QObject *parent) :
    QObject(parent), _version(0)
{
}

void Kernel::init() {
    if ( !_version.isNull())
        return;

    _version.reset(new Version());
    _version->addBinaryVersion(Ilwis::Version::bvFORMAT30);
    _version->addBinaryVersion(Ilwis::Version::bvFORMATFOREIGN);
    _version->addBinaryVersion(Ilwis::Version::bvPOLYGONFORMAT37);
    _version->addODFVersion("3.1");

    _issues.reset( new IssueLogger());

    _dbPublic = QSqlDatabase::addDatabase("QSQLITE");
    _dbPublic.setHostName("localhost");
    _dbPublic.setDatabaseName(":memory:");
    _dbPublic.open();

    _dbPublic.prepare();

    CatalogConnectorFactory *catfactory = new CatalogConnectorFactory();
    addFactory(catfactory);

    ConnectorFactory *confac = new ConnectorFactory();
    addFactory(confac);

    FeatureFactory *featureFac = new FeatureFactory();
    featureFac->addCreator("feature", createFeature);
    addFactory(featureFac);


    _modules.addModules();

    mastercatalog()->addContainer(QUrl("ilwis://system"));

   // ItemRange::addCreateItem("ThematicItem", ThematicItem::createRange());

}

Kernel::~Kernel() {
    _dbPublic.close();
}

const QVariant *Kernel::getFromTLS(const QString& key) const{
    if (_caches.hasLocalData()) {
        return _caches.localData()->object(key);
    }
    return 0;
}

void Kernel::setTLS(const QString& key, QVariant* data){
    if (!_caches.hasLocalData())
        _caches.setLocalData(new QCache<QString, QVariant>);

    _caches.localData()->insert(key, data);
}

void Kernel::deleteTLS(const QString &key) {
    if (_caches.hasLocalData())
        _caches.localData()->remove(key);

}

QString Kernel::translate(const QString& s) const {
    //TODO implement translator class here and load in in the application object
    return s;
}

 const SPVersion& Kernel::version() const{
     return _version;
 }

PublicDatabase &Kernel::database()
{
    return _dbPublic;
}


QScopedPointer<IssueLogger>& Kernel::issues()
{
    return _issues;
}

void Kernel::addFactory(FactoryInterface *fac)
{
    QString key = fac->key().toLower();
    if (!_masterfactory.contains(key)) {
        _masterfactory[key]  = fac;
    }
}

QString Kernel::demangle(const char *mangled_name)
{
    int status;
    char *realname = abi::__cxa_demangle(mangled_name,0,0,&status);
    QString type(realname);
    free(realname);

    return type;
}

bool Kernel::error(const QString &message, const QString p1, const QString p2, const QString p3)
{
    if ( p1 == sUNDEF)
        issues()->log(TR(message));
    if (p2 == sUNDEF)
        issues()->log(TR(message).arg(p1));
    else if ( p3 == sUNDEF)
        issues()->log(TR(message).arg(p1, p2));
    else
        issues()->log(TR(message).arg(p1).arg(p2).arg(p3));
    return false;
}

void Kernel::startClock(){
    _start_clock = clock();
}

void Kernel::endClock(){
    clock_t end = clock();
    double total = (double)(end - _start_clock) / CLOCKS_PER_SEC;
    qDebug() << "calc old in " << total << " seconds";

}





