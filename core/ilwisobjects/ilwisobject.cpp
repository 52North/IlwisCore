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
    _readOnly(false)
{
}

IlwisObject::IlwisObject(const Resource& ) :
    _valid(false),
    _readOnly(false)
{
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
        url = resource(cmode).url(true);
        if ( !url.isValid()){
            ERROR2(ERR_ILLEGAL_VALUE_2, "Url","");
            throw ErrorObject(TR(QString("illegal url %1 for format %2").arg(url.toString()).arg(format)));
        }
    }
    Resource res = resource();
    if ( !res.isValid()) {
        res = Resource(url,ilwisType(), false);
        res.setId(id());
    }
    if ( url != QUrl()) {
        QString currenturl = res.url().toString();
        // we dont replace the normalized urls for internal objects if the url is pointing to the (disk based) cache
        if ( !(currenturl.indexOf(INTERNAL_CATALOG) == 0 && !outurl.isValid())){
            res.setUrl(url);

        }
        if ( url.scheme() != "ilwis") // raw urls can never go to an ilwis scheme
            res.setUrl(url,true);
        else {
            // if we are going to the internalcatalog ensure that the physical path is good
            if ( url.toString().indexOf(INTERNAL_CATALOG) == 0){
                QString rawUrl = context()->persistentInternalCatalog().toString() + "/" + res.name();
                res.setUrl(rawUrl, true);
            }
        }


    }
    const Ilwis::ConnectorFactory *factory = kernel()->factory<Ilwis::ConnectorFactory>("ilwis::ConnectorFactory");
    if ( !factory)
        throw ErrorObject(TR(QString("couldnt find factory for %1").arg(format)));
    IOOptions opt = options;
    opt.addOption("format", format);
    Ilwis::ConnectorInterface *conn = factory->createFromFormat(res, format,fnamespace,opt);
    if (!conn){
        throw ErrorObject(TR(QString("couldnt connect to %1 datasource for %2").arg(format).arg(url.toString())));
    }
    setConnector(conn, cmode, options);
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
    if ( resource().url().toString().indexOf("ilwis://system/") == 0){
        _readOnly = true;
    }

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
    if ( !connector().isNull()){
        connector()->sourceRef().modifiedTime(Time::now());
        connector()->sourceRef().name(nm);
        if ( isInternalObject() ){
            QString path = context()->persistentInternalCatalog().toString() + "/" + nam;
            connector()->sourceRef().setUrl(path,true);
        }
    }
    if ( wasAnonymous && !isAnonymous()) // anonymous objects are not in the master table. If they now have a 'real' name it must be added to the mastercatalog
        if ( !isSystemObject()) // we dont add system objects this way
            mastercatalog()->addItems({resource()});
}

QString IlwisObject::name() const
{
    if ( !constConnector().isNull()) {
        return resource().name();
    }
    return sUNDEF;
}

void IlwisObject::code(const QString& cd) {
    if ( isReadOnly())
        return;
    changed(true);

    if ( !connector().isNull()){
        connector()->sourceRef().modifiedTime(Time::now());
        connector()->sourceRef().code(cd);
    }
}

QString IlwisObject::code() const
{
    if ( !constConnector().isNull())
        return constConnector()->sourceRef().code();
    return sUNDEF;
}

QString IlwisObject::description() const
{
    if ( !constConnector().isNull()){
        return constConnector()->sourceRef().description();
    }
    return sUNDEF;
}

void IlwisObject::setDescription(const QString &desc)
{
    if ( !connector().isNull()){
        connector()->sourceRef().modifiedTime(Time::now());
        connector()->sourceRef().setDescription(desc);
    }
}

void IlwisObject::addDescription(const QString &desc)
{
    if ( !connector().isNull()){
        connector()->sourceRef().modifiedTime(Time::now());
        QString descOld = connector()->sourceRef().description();
        if ( descOld != "")
            descOld += "\n";
        descOld += desc;
        connector()->sourceRef().setDescription(descOld);
    }
}

quint64 IlwisObject::id() const
{
    if ( !constConnector().isNull()){
        return constConnector()->sourceRef().id();
    }
    return i64UNDEF;
}

Time IlwisObject::modifiedTime() const
{
    if ( !constConnector().isNull())
        return constConnector()->sourceRef().modifiedTime() ;
    return tUNDEF;
}

void IlwisObject::modifiedTime(const Time &tme)
{

    if ( isReadOnly())
        return;
    if ( connector().isNull())
        return;
    changed(true);
    constConnector()->sourceRef().modifiedTime(tme) ;
}

Time IlwisObject::createTime() const
{
    if ( !constConnector().isNull())
        return constConnector()->sourceRef().createTime();
    return tUNDEF;
}

void IlwisObject::createTime(const Time &time)
{
    if ( isReadOnly())
        return;
    if ( connector().isNull())
        return;
   changed(true);
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
    changed(true);
    _valid = yesno;
    return _valid;
}

bool IlwisObject::isReadOnly() const
{
    return _readOnly;
}

bool IlwisObject::outputConnectionReadonly() const
{
    if ( !constConnector(cmOUTPUT).isNull())
        return constConnector(cmOUTPUT)->isReadOnly();
    return true;
}

QString IlwisObject::formatCode(bool input) const
{
    if ( input){
        if ( !constConnector().isNull()){
            return constConnector()->format();
        }
    }else {
        if(!constConnector(cmOUTPUT).isNull())
           return constConnector(cmOUTPUT)->format();
        else {
            return formatCode();
        }
    }

    return sUNDEF;
}

QString IlwisObject::provider(bool input) const
{
    if ( input){
        if ( !constConnector().isNull()){
            return constConnector()->provider();
        }
    }else {
        if(!constConnector(cmOUTPUT).isNull())
           return constConnector(cmOUTPUT)->provider();
        else {
            return provider()    ;
        }
    }

    return sUNDEF;
}

double IlwisObject::pseudoUndef() const
{
    return _pseudoUndef    ;
}

void IlwisObject::setPseudoUndef(double v)
{
    _pseudoUndef = v;
}

void IlwisObject::readOnly(bool yesno)
{
    if (!isSystemObject())
        _readOnly = yesno;
}

bool IlwisObject::hasChanged() const
{
    if ( _connector.isNull()){
        return _connector->source().hasChanged();
    }
    return false;
}

void IlwisObject::changed(bool yesno)
{
    if ( isReadOnly() || isSystemObject())
        return;
    if ( !connector().isNull()){
        return connector()->sourceRef().changed(yesno);
    }

}

IlwisTypes IlwisObject::extendedType() const
{
    if ( !_connector.isNull()){
        return _connector->source().extendedType();
    }
    return itUNKNOWN;
}

void IlwisObject::extendedType(quint64 extype)
{
    if ( !_connector.isNull()){
        return _connector->sourceRef().setExtendedType(extype);
    }
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


    if (mode & cmINPUT){
        quint64 pointer = (quint64) ( _connector.data());
        quint64 npointer = (quint64) ( connector);
        if ( pointer != npointer || npointer == 0){
            _connector.reset(connector);
            if ( !_connector.isNull()){
                bool ok = true;
                connector->addProperty("connectormode",IlwisObject::cmINPUT);
                if ( !(options.contains("create") && options.find("create").value().toBool() == true))
                    ok = _connector->loadMetaData(this, options);
                changed(false);
                return ok;
            }
        }
        else {
            kernel()->issues()->log(QString("Duplicate (out)connector assignement for input/output in %1").arg(name()),IssueObject::itWarning);
        }
    }
    if ( mode == cmOUTPUT ){ // skip cmOUTPUt | cmINPUT;
        quint64 pointer = (quint64) ( _outConnector.data());
        quint64 npointer = (quint64) ( connector);
        if ( pointer != npointer || npointer == 0){
            connector->addProperty("connectormode",IlwisObject::cmOUTPUT);
            _outConnector.reset(connector);
        }
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

const QScopedPointer<ConnectorInterface> &IlwisObject::constConnector(int mode) const
{
    if (  mode & cmINPUT)
        return _connector;
    else if ( mode & cmOUTPUT)
        if ( _outConnector.data() != 0)
            return _outConnector;
    return _connector;
}

void IlwisObject::resetOutputConnector()
{
    _outConnector.reset(0);
}

bool operator==(const IlwisObject& obj1, const IlwisObject& obj2) {
    return obj1.id() == obj2.id();
}

bool IlwisObject::fromInternal(const QSqlRecord &rec)
{
    if ( isReadOnly())
        return false;
    changed(true);

    code(rec.field("code").value().toString());
    auto v = rec.field("name").value().toString();
    if ( v == "")
        v = code();
    IlwisObject::name(v);
    setDescription(rec.field("description").value().toString());

    return true;
}

bool IlwisObject::isAnonymous() const
{
    return name().indexOf(ANONYMOUS_PREFIX) == 0;
}

Resource IlwisObject::resource(int mode) const
{
    if ( mode & cmINPUT || mode == cmEXTENDED) {
        if ( _connector.isNull() == false)
            return _connector->sourceRef();
        return Resource();
    } else if (mode & cmOUTPUT) {
        if ( _outConnector.isNull() == false) {
            return _outConnector->sourceRef();
        }
        else if ( _connector.isNull() == false)
            return _connector->sourceRef();
    }
    return Resource();
}

Resource& IlwisObject::resourceRef(int mode)
{
    if ( mode & cmINPUT || mode == cmEXTENDED) {
        if ( _connector.isNull() == false)
            return _connector->sourceRef();
    } else if (mode & cmOUTPUT) {
        if ( _outConnector.isNull() == false) {
            return _outConnector->sourceRef();
        }
        else if ( _connector.isNull() == false)
            return _connector->sourceRef();
    }
    throw InternalError(TR("Incomplete ilwis object used for referencing information"));
}

void IlwisObject::copyTo(IlwisObject *obj)
{
    obj->name(name());
    obj->code(code());
    obj->setDescription(description());
    obj->_valid = _valid;
    obj->_readOnly = _readOnly;
    const Ilwis::ConnectorFactory *factory = kernel()->factory<Ilwis::ConnectorFactory>("ilwis::ConnectorFactory");
    if ( !factory)
        return;
    Resource res = resource().copy(obj->id());
    if (!_connector.isNull()){
        Ilwis::ConnectorInterface *conn = factory->createFromResource(res, _connector->provider());
        obj->setConnector(conn, cmINPUT);
    }
    if ( !_outConnector.isNull()) {
        Ilwis::ConnectorInterface *conn = factory->createFromResource(res, _outConnector->provider());
        obj->setConnector(conn, cmOUTPUT);
    }
}

bool IlwisObject::isSystemObject() const {
    if ( code() == sUNDEF)
        return false;

    InternalDatabaseConnection db;
    QString query = QString("Select linkedtable from codes where code = '%1'").arg(code());
    bool ok = db.exec(query);
    if ( ok)
        return ok;
    QString path = resource().url().toString();
    return path.indexOf("ilwis://system/") == 0;
}

bool IlwisObject::isInternalObject() const
{
    if ( isAnonymous())
        return true;
    if (resource().isValid()){
        QString path = resource().url().toString();
        //worklfows nromalized url begins with ilwis but it not an internalobject
        bool ok = (path.indexOf("ilwis:/") == 0 && ilwisType() != itWORKFLOW) && path.indexOf("ilwis://system") == -1;
        return ok;
    }
    return false;
}

bool IlwisObject::isRemote() const
{
    if ( !resource().isValid())
        return false;

    if (resource().url().scheme() == "file" || resource().url(true).scheme() == "file")
        return false;
    if ( resource().url().scheme() == "ilwis" || resource().url(true).scheme() == "ilwis")
        return false;
    if (resource().url().scheme() == "http" || resource().url().scheme() == "https" || resource().url(true).scheme() == "http" || resource().url(true).scheme() == "https"){
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

void IlwisObject::unload()
{

}

bool IlwisObject::store(const IOOptions &options)
{
    if (!connector(cmOUTPUT).isNull()) {
        Locker<std::mutex> lock(_loadforstore);
        if (connector() && !connector()->dataIsLoaded()) {
            connector()->loadData(this, options);
        }
        bool ok = connector(cmOUTPUT)->store(this, options);
        changed(false);
        return ok;
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
    case  itCOMBINATIONMATRIX:
        return "CombinationMatrix";
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
    case  itSCRIPT:
        return "Script";
    case  itMODEL:
        return "Model";
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
    if ( name.compare( "NumericDomain",Qt::CaseInsensitive) == 0)
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
    if ( name.compare( "Script",Qt::CaseInsensitive) == 0) {
        return  itSCRIPT;
    }
    if ( name.compare( "Model",Qt::CaseInsensitive) == 0) {
        return  itMODEL;
    }
    if ( name.compare( "CombinationMatrix",Qt::CaseInsensitive) == 0) {
        return  itCOMBINATIONMATRIX;;
    }
    if ( name.compare( "OperationMetaData",Qt::CaseInsensitive) == 0) {
        return  itOPERATIONMETADATA;
    }
    if ( name.compare( "Catalog",Qt::CaseInsensitive) == 0)
        return  itCATALOG;
    if ( name.compare( "Representation",Qt::CaseInsensitive) == 0)
        return  itREPRESENTATION;
    if ( name.compare( "Column",Qt::CaseInsensitive) == 0)
        return  itCOLUMN;
    if ( name.compare( "string",Qt::CaseInsensitive) == 0)
        return  itSTRING;
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
    if ( name.compare( "bool",Qt::CaseInsensitive) == 0)
        return  itBOOL;
    if ( name.compare( "integer",Qt::CaseInsensitive) == 0)
        return  itINTEGER;
    if ( name.compare( "positiveinteger",Qt::CaseInsensitive) == 0)
        return  itPOSITIVEINTEGER;
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


