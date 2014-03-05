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
#include "ilwisdata.h"
#include "ilwiscontext.h"
#include "connectorfactory.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "filecatalogconnector.h"
#include "featurefactory.h"
#include "georefimplementationfactory.h"
#include "module.h"
#include "mastercatalog.h"
#include "version.h"
#include "errorobject.h"
#include "coverage.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "range.h"
#include "itemrange.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "operationExpression.h"
#include "commandhandler.h"
#include "operation.h"
#include "tranquilizer.h"

Ilwis::Kernel *Ilwis::Kernel::_kernel = 0;

using namespace Ilwis;

//Catalog *createCatalog()  {
//    return new Catalog();
//}

Ilwis::Kernel* Ilwis::kernel() {
    if (Kernel::_kernel == 0) {
        Kernel::_kernel = new Kernel();
        Kernel::_kernel->init();

    }
    return Kernel::_kernel;
}

bool Ilwis::initIlwis(QFileInfo configFile){
    try {
        context()->loadConfigFile(configFile);
        return kernel() != 0;
    } catch (const ErrorObject& err) {
        std::cout << err.message().toStdString();
    }
    return false;
}


Kernel::Kernel(QObject *parent) :
    QObject(parent), _version(0)
{
}

void Kernel::init() {

    if ( !_version.isNull())
        return;
     _issues.reset( new IssueLogger());

     issues()->log(QString("Ilwis started at %1").arg(Time::now().toString()),IssueObject::itMessage);

    _version.reset(new Version());
    _version->addBinaryVersion(Ilwis::Version::bvFORMAT30);
    _version->addBinaryVersion(Ilwis::Version::bvFORMATFOREIGN);
    _version->addBinaryVersion(Ilwis::Version::bvPOLYGONFORMAT37);
    _version->addODFVersion("3.1");



    _dbPublic = QSqlDatabase::addDatabase("QSQLITE");
    _dbPublic.setHostName("localhost");
    _dbPublic.setDatabaseName(":memory:");
    _dbPublic.open();

    _dbPublic.prepare();

    ConnectorFactory *confac = new ConnectorFactory();
    addFactory(confac);

    confac->addCreator(itCATALOG, "ilwis",FileCatalogConnector::create);


    FeatureFactory *featureFac = new FeatureFactory();
    featureFac->addCreator("feature", createFeature);
    addFactory(featureFac);

    GeoRefImplementationFactory *georefFac = new GeoRefImplementationFactory();
    georefFac->prepare();
    addFactory(georefFac);


    _modules.addModules();

    mastercatalog()->addContainer(QUrl("ilwis://internalcatalog"));

}

Kernel::~Kernel() {
    issues()->log(QString("Ilwis closed at %1").arg(Time::now().toString()),IssueObject::itMessage);
    _dbPublic.close();
    delete mastercatalog();
    delete context();
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
    //TODO: implement translator class here and load in in the application object
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

bool Kernel::message(const QString &message, IssueObject::IssueType tp, const QString p1, const QString p2, const QString p3, const QString& file,  int line, const QString& func)
{
    QFileInfo inf(file);
    QString name = inf.fileName();
    quint64 issueid;
    if ( p1 == sUNDEF)
        issueid =issues()->log(TR(message),tp);
    else if (p2 == sUNDEF)
        issueid =issues()->log(TR(message).arg(p1),tp);
    else if ( p3 == sUNDEF)
        issueid =issues()->log(TR(message).arg(p1, p2),tp);
    else
        issueid =issues()->log(TR(message).arg(p1).arg(p2).arg(p3),tp);
    if ( issueid != i64UNDEF) {
        issues()->addCodeInfo(issueid, line, func, name);
    }
    return false;
}

SPTranquilizer Kernel::createTrq(const QString& title, const QString& description, qint64 end, qint32 step) {
    std::shared_ptr<Tranquilizer> trq (new Tranquilizer(title, description, end, step));
    connect(trq.get(),&Tranquilizer::doMove, this, &Kernel::doProgress );
    return trq;
}

void Kernel::doProgress(quint64 id, qint32 amount)
{

}

void Kernel::startClock(){
     _start_clock = clock();
}

void Kernel::endClock(const QString& label){
    clock_t end = clock();
    double total = (double)(end - _start_clock) / CLOCKS_PER_SEC;
    if ( label == "")
        qDebug() << "calc old in " << total << " seconds";
    else
        qDebug() << label << ": "<< "calc old in " << QString::number(total,'g', 7) << " seconds";

}





