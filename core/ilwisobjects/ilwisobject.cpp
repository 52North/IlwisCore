#include <QUrl>
#include <QDateTime>
#include <QFileInfo>
#include <QSqlRecord>
#include <QSqlField>
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


IlwisObject *IlwisObject::create(const Resource& item) {

    const IlwisObjectFactory *factory = kernel()->factory<IlwisObjectFactory>("IlwisObjectFactory",item);

    if ( factory)
        return factory->create(item);
    else {
        kernel()->issues()->log(TR("Cann't find suitable factory for %1 ").arg(item.name()));
    }
    return 0;
}

void IlwisObject::connectTo(const QUrl& url, const QString& format, const QString& fnamespace, ConnectorMode cmode) {
    Locker lock(_mutex);
    Resource resource;
    resource = mastercatalog()->id2Resource(id());
    if ( !resource.isValid()) {
        resource = Resource(url,ilwisType(), false);
    }
    if ( url == QUrl()) {
        resource.setName(name());
    }
    const Ilwis::ConnectorFactory *factory = kernel()->factory<Ilwis::ConnectorFactory>("ilwis::ConnectorFactory");
    if ( !factory)
        return;
    Ilwis::ConnectorInterface *conn = factory->createFromFormat(resource, format,fnamespace);
    if (!conn)
        return;
    setConnector(conn, cmode);
    if ( name() == sUNDEF)
        setName(resource.name());

}

IlwisObject *IlwisObject::copy()
{
    return 0;
}


bool IlwisObject::prepare( ) {
    _valid = true;

    return true;
}

void IlwisObject::setName(const QString &nam)
{
    QString nm = nam;
    if ( nm == INTERNAL_PREFIX)
        nm += QString::number(id());
    Identity::setName(nm);
    if ( !connector().isNull())
        connector()->source().setName(nm);
}

void IlwisObject::setCode(const QString& cd) {
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
    _modifiedTime = tme;
}

Time IlwisObject::createTime() const
{
    return _createTime;
}

void IlwisObject::setCreateTime(const Time &time)
{
    _createTime = time;
}

QString IlwisObject::toString()
{
    //TODO
    return "";
}

bool IlwisObject::isEqual(const IlwisObject &obj) const
{
    //TODO overrule this method for object types that need to do more checking
    return id() == obj.id();
}



bool IlwisObject::isValid() const
{
    return _valid;
}

bool IlwisObject::setValid(bool yesno)
{
    _valid = yesno;
    return _valid;
}

bool IlwisObject::isReadOnly() const
{
    if ( !connector(cmOUTPUT).isNull())
        return connector(cmOUTPUT)->isReadOnly() && _readOnly;
    return _readOnly;

}

bool IlwisObject::hasChanged() const
{
    return _changed;
}

bool IlwisObject::prepare(const QString &)
{
    return true;
}

void IlwisObject::setConnector(ConnectorInterface *connector, ConnectorMode mode)
{
    if (mode == cmINPUT || mode == cmINOUT){
        quint64 pointer = (quint64) ( _outConnector.data());
        quint64 npointer = (quint64) ( connector);
        if ( pointer != npointer || npointer == 0)
            _connector.reset(connector);
        else {
            _valid = false;
            ERROR1("Duplicate connector assignement for input/output in %1", name());
        }
    }
    if ( mode == cmOUTPUT){
        quint64 pointer = (quint64) ( _connector.data());
        quint64 npointer = (quint64) ( connector);
        if ( pointer != npointer || npointer == 0)
         _outConnector.reset(connector);
        else {
            _valid = false;
            ERROR1("Duplicate connector assignement for input/output in %1", name());
        }
    }
}

QScopedPointer<ConnectorInterface> &IlwisObject::connector(ConnectorMode mode)
{
    if ( mode == cmINPUT)
        return _connector;
    else if ( mode == cmOUTPUT)
        if ( _outConnector.data() != 0)
            return _outConnector;
    return _connector;
}

const QScopedPointer<ConnectorInterface> &IlwisObject::connector(ConnectorMode mode) const
{
    if (  mode == cmINPUT)
        return _connector;
    else if ( mode == cmOUTPUT)
        if ( _outConnector.data() != 0)
            return _outConnector;
    return _connector;
}

bool operator==(const IlwisObject& obj1, const IlwisObject& obj2) {
    return obj1.id() == obj2.id();
}

bool IlwisObject::fromInternal(const QSqlRecord &rec)
{
    setName(rec.field("code").value().toString()); // name and code are the same here
    setDescription(rec.field("description").value().toString());
    setCode(rec.field("code").value().toString());
    setConnector(0);

    return true;
}

bool IlwisObject::isInternal() const
{
    return name().indexOf(INTERNAL_PREFIX) == 0;
}

Resource IlwisObject::source() const
{
    if ( _connector.isNull() == false)
        return _connector->source();
    return Resource();
}

Resource IlwisObject::target() const
{
    if ( _outConnector.isNull())
        return Resource();
    return _outConnector->source();
}

bool IlwisObject::storeMetaData() {
    if ( !connector(cmOUTPUT).isNull())
        return connector(cmOUTPUT)->storeMetaData(this);
    return ERROR1(ERR_NO_INITIALIZED_1,"connector");

}
bool IlwisObject::storeBinaryData() {
    if (!connector(cmOUTPUT).isNull())
        return connector(cmOUTPUT)->storeBinaryData(this);
    return ERROR1(ERR_NO_INITIALIZED_1,"connector");
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
        Ilwis::ConnectorInterface *conn = factory->createFromResource(resource, _connector->provider());
        obj->setConnector(conn, cmOUTPUT);
    }
}

bool IlwisObject::store(int storemode)
{
    bool ok = true;
    if ( storemode & smMETADATA)
        ok &= storeMetaData();
    if ( storemode & smBINARYDATA)
        ok &= storeBinaryData();

    return ok;
}

//------ statics ----------------------------------------------

QString IlwisObject::type2Name(IlwisTypes t)
{
    switch(t) {
    case  itGRID:
        return "GridCoverage";
    case  itPOLYGON:
        return "PolygonCoverage";
    case  itLINE:
        return "LineCoverage";
    case  itPOINT:
        return "PointCoverage";
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
    case  itCORNERSGEOREF:
        return "CornersGeoreference";
    case  itTIEPOINTGEOREF:
        return "TiePointsCornersGeoreference";
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

IlwisTypes IlwisObject::name2Type(const QString& dname)
{
    QString name = dname;
    int index;
    if ( (index = name.indexOf("::")) != -1){
        name = dname.right(name.size() - index - 2);
    }

    if ( name.compare("GridCoverage",Qt::CaseInsensitive) == 0)
        return  itGRID;
    if ( name.compare( "PolygonCoverage",Qt::CaseInsensitive) == 0)
        return  itPOLYGON;
    if ( name.compare( "LineCoverage",Qt::CaseInsensitive) == 0)
        return  itLINE;
    if ( name.compare( "PointCoverage",Qt::CaseInsensitive) == 0)
        return  itPOINT;
    if ( name.compare( "FeatureCoverage",Qt::CaseInsensitive) == 0)
        return  itFEATURECOVERAGE;
    if ( name.compare( "ValueDomain",Qt::CaseInsensitive) == 0)
        return  itNUMERICDOMAIN;
    if ( name.mid(0,10) == "ItemDomain") // contains template construct, so different comparison
        return  itITEMDOMAIN;
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



