#include <QUrl>
#include <QDateTime>
#include <QFileInfo>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include "kernel.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"
#include "ilwisdata.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
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
    _valid(false),
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


IlwisObject *IlwisObject::create(const Resource& resource, const IOOptions &options) {

    const IlwisObjectFactory *factory = kernel()->factory<IlwisObjectFactory>("IlwisObjectFactory",resource,options);

    if ( factory)
        return factory->create(resource, options);
    else {
        kernel()->issues()->log(TR("Cann't find suitable factory for %1 ").arg(resource.name()));
    }
    return 0;
}

void IlwisObject::connectTo(const QUrl& outurl, const QString& format, const QString& fnamespace, ConnectorMode cmode, const IOOptions& options) {

    Locker<> lock(_mutex);
    if ( isReadOnly())
        return throw ErrorObject(TR(QString("format %1 or data object is readonly").arg(format)));

    QUrl url(outurl);
    if (!url.isValid()){
        url = source(cmode).url(true);
        if ( !url.isValid()){
            ERROR2(ERR_ILLEGAL_VALUE_2, "Url","");
            throw ErrorObject(TR(QString("illegal url %1 for format %2").arg(url.toString()).arg(format)));
        }
    }



    Resource resource;
    resource = mastercatalog()->id2Resource(id());
    if ( !resource.isValid()) {
        resource = Resource(url,ilwisType(), false);
        resource.setId(id());
    }
    if ( url != QUrl()) {
        QString currenturl = resource.url().toString();
        // we dont replace the normalized urls for internal objects if the url is pointing to the (disk based) cache
        if ( !(currenturl.indexOf("ilwis://internalcatalog") == 0 && !outurl.isValid()))
            resource.setUrl(url);
        resource.setUrl(url,true);
    }
    const Ilwis::ConnectorFactory *factory = kernel()->factory<Ilwis::ConnectorFactory>("ilwis::ConnectorFactory");
    if ( !factory)
        throw ErrorObject(TR(QString("couldnt find factory for %1").arg(format)));
    IOOptions opt = options;
    opt.addOption("format", format);
    Ilwis::ConnectorInterface *conn = factory->createFromFormat(resource, format,fnamespace,opt);
    if (!conn){
        throw ErrorObject(TR(QString("couldnt connect to %1 datasource for %2").arg(format).arg(url.toString())));
    }
    setConnector(conn, cmode, options);
    if ( Identity::name() == sUNDEF)
        name(resource.name());

}

IlwisObject *IlwisObject::clone()
{
    return 0;
}

bool IlwisObject::canUse(const IlwisObject *, bool ) const
{
    return false;
}

bool IlwisObject::isCompatibleWith(const IlwisObject*, bool strict) const
{
    return false;
}

bool IlwisObject::merge(const IlwisObject *, int )
{
    return false;
}


bool IlwisObject::prepare(const Ilwis::IOOptions &) {
    _valid = true;

    return true;
}

void IlwisObject::name(const QString &nam)
{
    if ( isReadOnly())
        return;

    bool wasAnonymous = isAnonymous();
    QString nm = nam;
    if ( nm == ANONYMOUS_PREFIX)
        nm += QString::number(id());
    Identity::name(nm);
    if ( !connector().isNull()){
        connector()->source().name(nm);
        if ( isInternalObject()){
            QString path = context()->persistentInternalCatalog().toString() + "/" + nam;
            connector()->source().setUrl(path,true);
        }
    }
    if ( wasAnonymous && !isAnonymous()) // anonymous objects are not in the master table. If they now have a 'real' name it must be added to the mastercatalog
        mastercatalog()->addItems({source()});
}

void IlwisObject::code(const QString& cd) {
    if ( isReadOnly())
        return;
    _changed = true;

    Identity::code(cd);
    if ( !connector().isNull())
        connector()->source().code(cd);
}

Time IlwisObject::modifiedTime() const
{
    return _modifiedTime;
}

void IlwisObject::modifiedTime(const Time &tme)
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

void IlwisObject::createTime(const Time &time)
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
    return _readOnly;
}

bool IlwisObject::outputConnectionReadonly() const
{
    if ( !connector(cmOUTPUT).isNull())
        return connector(cmOUTPUT)->isReadOnly();
    return true;
}

QString IlwisObject::formatCode(bool input) const
{
    if ( input){
        if ( !connector().isNull()){
            return connector()->format();
        }
    }else {
        if(!connector(cmOUTPUT).isNull())
           return connector(cmOUTPUT)->format();
        else {
            return formatCode();
        }
    }

    return sUNDEF;
}

QString IlwisObject::provider(bool input) const
{
    if ( input){
        if ( !connector().isNull()){
            return connector()->provider();
        }
    }else {
        if(!connector(cmOUTPUT).isNull())
           return connector(cmOUTPUT)->provider();
        else {
            return provider()    ;
        }
    }

    return sUNDEF;
}

void IlwisObject::readOnly(bool yesno)
{
    if (!isSystemObject())
        _readOnly = yesno;
}

bool IlwisObject::hasChanged() const
{
    return _changed;
}

void IlwisObject::changed(bool yesno)
{
    if ( isReadOnly() || isSystemObject())
        return;

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

bool IlwisObject::setConnector(ConnectorInterface *connector, int mode, const IOOptions &options)
{
    if ( isReadOnly())
        return false;
    _changed = true;

    if (mode & cmINPUT){
        quint64 pointer = (quint64) ( _connector.data());
        quint64 npointer = (quint64) ( connector);
        if ( pointer != npointer || npointer == 0){
            _connector.reset(connector);
            if ( !_connector.isNull())
                return _connector->loadMetaData(this, options);
        }
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
    return true;
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

    name(rec.field("code").value().toString()); // name and code are the same here
    setDescription(rec.field("description").value().toString());
    code(rec.field("code").value().toString());
    setConnector(0);

    return true;
}

bool IlwisObject::isAnonymous() const
{
    return name().indexOf(ANONYMOUS_PREFIX) == 0;
}

Resource IlwisObject::source(int mode) const
{
    if ( mode & cmINPUT || mode == cmEXTENDED) {
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
    obj->name(name());
    obj->code(code());
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
    InternalDatabaseConnection db;
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

bool IlwisObject::isRemote() const
{
    if ( !source().isValid())
        return false;

    if (source().url().scheme() == "file" || source().url(true).scheme() == "file")
        return false;
    if ( source().url().scheme() == "ilwis" || source().url(true).scheme() == "ilwis")
        return false;
    if (source().url().scheme() == "http" || source().url().scheme() == "https" || source().url(true).scheme() == "http" || source().url(true).scheme() == "https"){
        return true;
    }
    return false;
}

void IlwisObject::remove()
{
    if (!connector().isNull()){
        connector()->removeDataSource();
    }
}

bool IlwisObject::store(const IOOptions &options)
{
    if (!connector(cmOUTPUT).isNull()) {
        Locker<std::mutex> lock(_loadforstore);
        if (connector() && !connector()->dataIsLoaded()) {
            connector()->loadData(this, options);
        }
        return connector(cmOUTPUT)->store(this, options);
    }

    return ERROR1(ERR_NO_INITIALIZED_1,"connector");
}

bool IlwisObject::loadData(const IOOptions& options)
{
    if ( connector().isNull())
        return false;
    if ( connector()->dataIsLoaded())
        return false;
    return connector()->loadData(this, options);

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
    case itCOVERAGE:
        return "Coverage";
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
    case  itCOLORDOMAIN:
        return "ColorDomain";
    case  itCOORDSYSTEM:
        return "CoordinateSystem";
    case  itCONVENTIONALCOORDSYSTEM:
        return "ConventionalCoordinateSystem";
    case itBOUNDSONLYCSY:
        return "BoundsOnlyCoordinateSystem"        ;
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
    case  itREPRESENTATION:
        return "Representation";
    case  itWORKFLOW:
        return "Workflow";
    case  itSINGLEOPERATION:
        return "SingleOperation";
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
    if ( name == "ItemDomain<Ilwis::Interval>")
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

    if ( name.compare("IlwisObject",Qt::CaseInsensitive) == 0)
        return  itILWISOBJECT;
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
    if ( name.compare( "Coverage",Qt::CaseInsensitive) == 0)
        return  itCOVERAGE;
    if ( name.mid(0,10) == "ItemDomain") // contains template construct, so different comparison
        return  itITEMDOMAIN;
    if ( name == "NumericDomain") // contains template construct, so different comparison
        return  itNUMERICDOMAIN;
    if ( name.compare( "TextDomain",Qt::CaseInsensitive) == 0)
        return  itTEXTDOMAIN;
    if ( name.compare( "ColorDomain",Qt::CaseInsensitive) == 0)
        return  itCOLORDOMAIN;
    if ( name.compare( "Domain",Qt::CaseInsensitive) == 0)
        return  itDOMAIN;
    if ( name.compare( "CoordinateSystem",Qt::CaseInsensitive) == 0)
        return  itCOORDSYSTEM;
    if ( name.compare( "ConventionalCoordinateSystem",Qt::CaseInsensitive) == 0)
        return  itCONVENTIONALCOORDSYSTEM;
    if ( name.compare( "BoundsOnlyCoordinateSystem",Qt::CaseInsensitive) == 0)
        return  itBOUNDSONLYCSY;
    if ( name.compare( "Georeference",Qt::CaseInsensitive) == 0)
        return  itGEOREF;
    if ( name.compare( "Table",Qt::CaseInsensitive) == 0)
        return  itTABLE;
    if ( name.compare( "FlatTable",Qt::CaseInsensitive) == 0)
        return  itFLATTABLE;
    if ( name.compare( "AttributeTable",Qt::CaseInsensitive) == 0)
        return  itATTRIBUTETABLE;
    if ( name.compare( "Projection",Qt::CaseInsensitive) == 0)
        return  itPROJECTION;
    if ( name.compare( "Ellipsoid",Qt::CaseInsensitive) == 0)
        return  itELLIPSOID;
    if ( name.compare( "GeodeticDatum",Qt::CaseInsensitive) == 0)
        return  itGEODETICDATUM;
    if ( name.compare( "Catalog",Qt::CaseInsensitive) == 0)
        return  itCATALOG;
    if ( name.compare( "SingleOperation",Qt::CaseInsensitive) == 0)
        return  itSINGLEOPERATION;
    if ( name.compare( "Workflow",Qt::CaseInsensitive) == 0) {
        return  itWORKFLOW;
    }
    if ( name.compare( "Catalog",Qt::CaseInsensitive) == 0)
        return  itCATALOG;
    if ( name.compare( "Representation",Qt::CaseInsensitive) == 0)
        return  itREPRESENTATION;
    // standard c++ types
    if ( name.compare( "int",Qt::CaseInsensitive) == 0)
        return  itINT32;
    if ( name.compare( "long",Qt::CaseInsensitive) == 0)
        return  itINT32;
    if ( name.compare( "unsigned int",Qt::CaseInsensitive) == 0)
        return  itUINT32;
    if ( name.compare( "unsigned long",Qt::CaseInsensitive) == 0)
        return  itUINT32;
    if ( name.compare( "short",Qt::CaseInsensitive) == 0)
        return  itUINT16;
    if ( name.compare( "unsigned short",Qt::CaseInsensitive) == 0)
        return  itUINT16;
    if ( name.compare( "char",Qt::CaseInsensitive) == 0)
        return  itINT8;
    if ( name.compare( "unsigned char",Qt::CaseInsensitive) == 0)
        return  itUINT8;
    if ( name.compare( "double",Qt::CaseInsensitive) == 0)
        return  itDOUBLE;
    if ( name.compare( "float",Qt::CaseInsensitive) == 0)
        return  itFLOAT;
    if ( name.compare( "char const *",Qt::CaseInsensitive) == 0)
        return  itSTRING;
    if ( name.compare( "char *",Qt::CaseInsensitive) == 0)
        return  itSTRING;



    return itUNKNOWN;

}

quint64 IlwisObject::internalname2id(const QString &name, bool resolve)
{
    int index = 0;
    QString sid = sUNDEF;
    if ( (index = name.indexOf(NAME_ALIAS)) != -1) {
        sid = name.mid(index + SZ_NAME_ALIAS);
    }
    else if ( (index = name.indexOf(ANONYMOUS_PREFIX)) != -1) { // internal objects are not in the catalog
        sid = name.mid(index + SZ_ANONYMOUS_PREFIX);
    }
    bool ok;
    quint64 id = sid.toLongLong(&ok);
    if (ok){
        if (!resolve)
            return id;
        ESPIlwisObject data = mastercatalog()->get(id);
        if ( data.get() != 0) {
            return data->id();
        }
    }
    return i64UNDEF;

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


