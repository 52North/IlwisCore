#include <QUrl>
#include <QDateTime>
#include <QFileInfo>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include "ilwis.h"
#include "kernel.h"
#include "issuelogger.h"
#include "connectorinterface.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"
#include "ilwisdata.h"
#include "resource.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "ilwisobjectconnector.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "version.h"



using namespace Ilwis;

QVector<IlwisTypeFunction> IlwisObject::_typeFunctions;

//-------------------------------------------------

IlwisObject::IlwisObject() :
    _valid(false),
    _readOnly(false),
    _changed(false)
{
    Identity::prepare();
}

IlwisObject::IlwisObject(const Resource& resource) :
    Identity(resource.name(), resource.id(), resource.code(), resource.description()) ,
    _readOnly(false),
    _changed(false)
{
    if (!resource.isValid())
        Identity::prepare();
}

IlwisObject::~IlwisObject()
{
      // qDeleteAll(_factories);
}


IlwisObject *IlwisObject::create(const Resource& resource) {

    const IlwisObjectFactory *factory = kernel()->factory<IlwisObjectFactory>("IlwisObjectFactory",resource);

    if ( factory)
        return factory->create(resource);
    else {
        kernel()->issues()->log(TR("Cann't find suitable factory for %1 ").arg(resource.name()));
    }
    return 0;
}

bool IlwisObject::connectTo(const QUrl& url, const QString& format, const QString& fnamespace, ConnectorMode cmode) {
    Locker lock(_mutex);
    if (!url.isValid()){
        ERROR2(ERR_ILLEGAL_VALUE_2, "Url","");
        return false;
    }

    if ( isReadOnly())
        return false;
    Resource resource;
    resource = mastercatalog()->id2Resource(id());
    if ( !resource.isValid()) {
        resource = Resource(url,ilwisType(), false);
        resource.setId(id());
    }
    if ( url != QUrl()) {
        resource.setUrl(url);
    }
    const Ilwis::ConnectorFactory *factory = kernel()->factory<Ilwis::ConnectorFactory>("ilwis::ConnectorFactory");
    if ( !factory)
        return false;
    Ilwis::ConnectorInterface *conn = factory->createFromFormat(resource, format,fnamespace);
    if (!conn)
        return false;
    setConnector(conn, cmode);
    if ( name() == sUNDEF)
        setName(resource.name());

    return true;

}

IlwisObject *IlwisObject::clone()
{
    return 0;
}

bool IlwisObject::merge(const IlwisObject *, int )
{
    return false;
}


bool IlwisObject::prepare( ) {
    _valid = true;

    return true;
}

void IlwisObject::setName(const QString &nam)
{
    if ( isReadOnly())
        return;

    QString nm = nam;
    if ( nm == ANONYMOUS_PREFIX)
        nm += QString::number(id());
    Identity::setName(nm);
    if ( !connector().isNull())
        connector()->source().setName(nm);
}

void IlwisObject::setCode(const QString& cd) {
    if ( isReadOnly())
        return;
    _changed = true;

    Identity::setCode(cd);
    if ( !connector().isNull())
        connector()->source().setCode(cd);
}

QDateTime IlwisObject::modifiedTime() const
{
    return _modifiedTime;
}

void IlwisObject::setModifiedTime(const Time &tme)
{
    if ( isReadOnly())
        return;
    _changed = true;
    _modifiedTime = tme;
}

Time IlwisObject::createTime() const
{
    return _createTime;
}

void IlwisObject::setCreateTime(const Time &time)
{
    if ( isReadOnly())
        return;
    _changed = true;
    _createTime = time;
}

QString IlwisObject::toString()
{
    //TODO:
    return "";
}

bool IlwisObject::isEqual(const IlwisObject *obj) const
{
    //TODO: overrule this method for object types that need to do more checking
    if ( obj == 0)
        return false;

    return id() == obj->id();
}



bool IlwisObject::isValid() const
{
    return _valid;
}

bool IlwisObject::setValid(bool yesno)
{
    if ( isReadOnly())
        return _valid;
    _changed = true;
    _valid = yesno;
    return _valid;
}

bool IlwisObject::isReadOnly() const
{
    if ( !connector(cmOUTPUT).isNull())
        return connector(cmOUTPUT)->isReadOnly() && _readOnly;
    return _readOnly;

}

void IlwisObject::setReadOnly(bool yesno)
{

    _readOnly = yesno;
}

bool IlwisObject::hasChanged() const
{
    return _changed;
}

void IlwisObject::changed(bool yesno)
{
    _modifiedTime = Time::now();
    _changed = yesno;
}

bool IlwisObject::prepare(const QString &)
{
    if ( isReadOnly())
        return false;
    changed(true);
    return true;
}

void IlwisObject::setConnector(ConnectorInterface *connector, int mode)
{
    if ( isReadOnly())
        return;
    _changed = true;

    if (mode & cmINPUT){
        quint64 pointer = (quint64) ( _outConnector.data());
        quint64 npointer = (quint64) ( connector);
        if ( pointer != npointer || npointer == 0)
            _connector.reset(connector);
        else {
            kernel()->issues()->log(QString("Duplicate (out)connector assignement for input/output in %1").arg(name()),IssueObject::itWarning);
        }
    }
    if ( mode == cmOUTPUT ){ // skip cmOUTPUt | cmINPUT;
        quint64 pointer = (quint64) ( _outConnector.data());
        quint64 npointer = (quint64) ( connector);
        if ( pointer != npointer || npointer == 0)
            _outConnector.reset(connector);
        else {
            kernel()->issues()->log(QString("Duplicate (out)connector assignement for input/output in %1").arg(name()),IssueObject::itWarning);
        }
    }
}

QScopedPointer<ConnectorInterface> &IlwisObject::connector(int mode)
{
    if ( mode & cmINPUT)
        return _connector;
    else if ( mode & cmOUTPUT)
        if ( _outConnector.data() != 0)
            return _outConnector;
    return _connector;
}

const QScopedPointer<ConnectorInterface> &IlwisObject::connector(int mode) const
{
    if (  mode & cmINPUT)
        return _connector;
    else if ( mode & cmOUTPUT)
        if ( _outConnector.data() != 0)
            return _outConnector;
    return _connector;
}

bool operator==(const IlwisObject& obj1, const IlwisObject& obj2) {
    return obj1.id() == obj2.id();
}

bool IlwisObject::fromInternal(const QSqlRecord &rec)
{
    if ( isReadOnly())
        return false;
    _changed = true;

    setName(rec.field("code").value().toString()); // name and code are the same here
    setDescription(rec.field("description").value().toString());
    setCode(rec.field("code").value().toString());
    setConnector(0);

    return true;
}

bool IlwisObject::isAnonymous() const
{
    return name().indexOf(ANONYMOUS_PREFIX) == 0;
}

Resource IlwisObject::source(int mode) const
{
    if ( mode & cmINPUT) {
        if ( _connector.isNull() == false)
            return _connector->source();
        return Resource();
    } else if (mode & cmOUTPUT) {
        if ( _outConnector.isNull() == false) {
            return _outConnector->source();
        }
        else if ( _connector.isNull() == false)
            return _connector->source();
    }
    return Resource();
}

void IlwisObject::copyTo(IlwisObject *obj)
{
    obj->setName(name());
    obj->setCode(code());
    obj->setDescription(description());
    obj->_valid = _valid;
    obj->_readOnly = _readOnly;
    obj->_changed = _changed;
    obj->_modifiedTime = Time::now();
    obj->_createTime = Time::now();
    const Ilwis::ConnectorFactory *factory = kernel()->factory<Ilwis::ConnectorFactory>("ilwis::ConnectorFactory");
    if ( !factory)
        return;
    Resource resource = source().copy(obj->id());
    if (!_connector.isNull()){
        Ilwis::ConnectorInterface *conn = factory->createFromResource(resource, _connector->provider());
        obj->setConnector(conn, cmINPUT);
    }
    if ( !_outConnector.isNull()) {
        Ilwis::ConnectorInterface *conn = factory->createFromResource(resource, _outConnector->provider());
        obj->setConnector(conn, cmOUTPUT);
    }
}

bool IlwisObject::isSystemObject() const {
    QSqlQuery db(kernel()->database());
    QString query = QString("Select linkedtable from codes where code = '%1'").arg(code());
    return db.exec(query) &&  db.next();
}

bool IlwisObject::isInternalObject() const
{
    if ( isAnonymous())
        return true;
    if (source().isValid()){
        return source().url().scheme() == "ilwis";
    }
    return false;
}

bool IlwisObject::store(int storemode)
{
    if (!connector(cmOUTPUT).isNull()) {
        Locker lock(_loadforstore);
        if (connector() && !connector()->binaryIsLoaded()) {
            connector()->loadBinaryData(this);
        }
        return connector(cmOUTPUT)->store(this, storemode);
    }

    return ERROR1(ERR_NO_INITIALIZED_1,"connector");
}

//------ statics ----------------------------------------------

QString IlwisObject::type2Name(IlwisTypes t)
{
    switch(t) {
    case  itRASTER:
        return "RasterCoverage";
    case  itPOLYGON:
        return "PolygonCoverage";
    case  itLINE:
        return "LineCoverage";
    case  itPOINT:
        return "PointCoverage";
    case  itPOINT+itLINE:
        return "FeatureCoverage";
    case  itPOINT+itPOLYGON:
        return "FeatureCoverage";
    case  itPOLYGON+itLINE:
        return "FeatureCoverage";
    case  itPOINT+itLINE+itPOLYGON:
        return "FeatureCoverage";
    case  itNUMERICDOMAIN:
        return "ValueDomain";
    case  itITEMDOMAIN:
        return "ItemDomain";
    case  itTIMEDOMAIN:
        return "TimeDomain";
    case  itCOORDSYSTEM:
        return "CoordinateSystem";
    case  itCONVENTIONALCOORDSYSTEM:
        return "ConventionalCoordinateSystem";
    case  itGEOREF:
        return "Georeference";
    case  itTABLE:
        return "Table";
    case  itPROJECTION:
        return "Projection";
    case  itELLIPSOID:
        return "Ellipsoid";
    case  itGEODETICDATUM:
        return "GeodeticDatum";
    case  itCATALOG:
        return "Catalog";
    case  itOPERATIONMETADATA:
        return "OperationMetaData";
    }
    return sUNDEF;

}

IlwisTypes IlwisObject::name2ExtendedType(const QString& dname) {
    QString name = dname;
    int index;
    if ( (index = name.indexOf("::")) != -1){
        name = dname.right(name.size() - index - 2);
    }
    if ( name == "ItemDomain<Ilwis::NamedIdentifier>")
        return  itNAMEDITEM;
    if ( name == "ItemDomain<Ilwis::IndexedIdentifier>")
        return  itINDEXEDITEM;
    if ( name == "ItemDomain<Ilwis::ThematicItem>")
        return  itTHEMATICITEM;
    if ( name == "ItemDomain<Ilwis::NumericItem>")
        return  itNUMERICITEM;
    return itUNKNOWN;
}

IlwisTypes IlwisObject::name2Type(const QString& dname)
{
    QString name = dname;
    int index;
    if ( (index = name.indexOf("::")) != -1){
        name = dname.right(name.size() - index - 2);
    }

    if ( name.compare("RasterCoverage",Qt::CaseInsensitive) == 0)
        return  itRASTER;
    if ( name.compare( "PolygonCoverage",Qt::CaseInsensitive) == 0)
        return  itPOLYGON;
    if ( name.compare( "LineCoverage",Qt::CaseInsensitive) == 0)
        return  itLINE;
    if ( name.compare( "PointCoverage",Qt::CaseInsensitive) == 0)
        return  itPOINT;
    if ( name.compare( "FeatureCoverage",Qt::CaseInsensitive) == 0)
        return  itFEATURE;
    if ( name.mid(0,10) == "ItemDomain") // contains template construct, so different comparison
        return  itITEMDOMAIN;
    if ( name == "NumericDomain") // contains template construct, so different comparison
        return  itNUMERICDOMAIN;
    if ( name.compare( "TextDomain",Qt::CaseInsensitive) == 0)
        return  itTEXTDOMAIN;
    if ( name.compare( "Domain",Qt::CaseInsensitive) == 0)
        return  itDOMAIN;
    if ( name.compare( "CoordinateSystem",Qt::CaseInsensitive) == 0)
        return  itCOORDSYSTEM;
    if ( name.compare( "ConventionalCoordinateSystem",Qt::CaseInsensitive) == 0)
        return  itCONVENTIONALCOORDSYSTEM;
    if ( name.compare( "Georeference",Qt::CaseInsensitive) == 0)
        return  itGEOREF;
    if ( name.compare( "Table",Qt::CaseInsensitive) == 0)
        return  itTABLE;
    if ( name.compare( "FlatTable",Qt::CaseInsensitive) == 0)
        return  itFLATTABLE;
    if ( name.compare( "DatabaseTable",Qt::CaseInsensitive) == 0)
        return  itDATABASETABLE;
    if ( name.compare( "Projection",Qt::CaseInsensitive) == 0)
        return  itPROJECTION;
    if ( name.compare( "Ellipsoid",Qt::CaseInsensitive) == 0)
        return  itELLIPSOID;
    if ( name.compare( "GeodeticDatum",Qt::CaseInsensitive) == 0)
        return  itGEODETICDATUM;
    if ( name.compare( "Catalog",Qt::CaseInsensitive) == 0)
        return  itCATALOG;
    if ( name.compare( "OperationMetaData",Qt::CaseInsensitive) == 0)
        return  itOPERATIONMETADATA;
    if ( name.compare( "OperationMetaData",Qt::CaseInsensitive) == 0)
        return  itOPERATIONMETADATA;


    return itUNKNOWN;

}

void IlwisObject::addTypeFunction(IlwisTypeFunction func)
{
    _typeFunctions.push_back(func);
}

IlwisTypes IlwisObject::findType(const QString &resource)
{
    foreach(IlwisTypeFunction func, _typeFunctions) {
        IlwisTypes tp = func(resource);
        if ( tp != itUNKNOWN)
            return tp;
    }
    return itUNKNOWN;
}


