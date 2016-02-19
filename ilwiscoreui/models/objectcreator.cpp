#include "resourcemodel.h"
#include "ilwisobjectcreatormodel.h"
#include "ilwiscontext.h"
#include "domain.h"
#include "geometries.h"
#include "projection.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "numericdomain.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "workflow.h"
#include "commandhandler.h"
#include "uicontextmodel.h"
#include "mastercatalogmodel.h"
#include "operationcatalogmodel.h"
#include "objectcreator.h"
#include "operationmetadata.h"

using namespace Ilwis;

ObjectCreator::ObjectCreator(QObject *parent) : QObject(parent)
{
    _creators["workflow"] = new IlwisObjectCreatorModel("workflow",TR("Workflow"),itWORKFLOW,"CreateWorkflow.qml", 400, this);
    _creators["numericdomain" ] = new IlwisObjectCreatorModel("numericdomain",TR("Numeric Domain"),itNUMERICDOMAIN,"CreateNumDom.qml", 250, this);
    _creators["thematicdomain" ] = new IlwisObjectCreatorModel("thematicdomain", TR("Thematic Domain"),itITEMDOMAIN | itTHEMATICITEM,"CreateThematicDom.qml", 520, this);
    _creators["nameidentifierdomain" ] = new IlwisObjectCreatorModel("nameidentifierdomain",TR("Identifier Domain"),itITEMDOMAIN | itIDENTIFIERITEM,"CreateIdentifierDomain.qml", 520, this);
    //_creators[ ] = new IlwisObjectCreatorModel(TR("Indexed Domain",itITEMDOMAIN | itINDEXEDITEM,"CreateNumDom.qml", 200, this);
    _creators["intervaldomain" ] = new IlwisObjectCreatorModel("intervaldomain",TR("Interval Domain"),itITEMDOMAIN | itNUMERICITEM,"CreateIntervalDomain.qml", 550, this);
    _creators["timedomain" ] = new IlwisObjectCreatorModel("timedomain",TR("Time Domain"),itTIME | itDOMAIN,"CreateNumDom.qml", 200, this);
    _creators["timeintervaldomain" ] = new IlwisObjectCreatorModel("timeintervaldomain", TR("Time Interval Domain"),itTIMEITEM | itITEMDOMAIN,"CreateNumDom.qml", 200, this);
//    _creators[ ] = new IlwisObjectCreatorModel(TR("Color Domain",itCOLORDOMAIN,"CreateNumDom.qml", 200, this);
    _creators["colorpalette" ] = new IlwisObjectCreatorModel("colorpalette", TR("Color Palette Domain"),itPALETTECOLOR | itITEMDOMAIN,"CreatePaletteDomain.qml", 560, this);
    _creators["cornersgeoreferences" ] = new IlwisObjectCreatorModel("cornersgeoreferences", TR("Corners Georeference"),itGEOREF,"CreateGeorefCorners.qml", 350, this);
    _creators["tiepointgeoreference" ] = new IlwisObjectCreatorModel("tiepointgeoreference",TR("Tiepoints Georeference"),itGEOREF | itLOCATION,"CreateGeorefTiepoints.qml", 280, this);
    _creators["projectedcoordinatesystem" ] = new IlwisObjectCreatorModel("projectedcoordinatesystem", TR("Projected Coordinate System"),itCONVENTIONALCOORDSYSTEM,"CreateProjectedCoordinateSystem.qml", 500, this);
    _creators["geographiccoordinatesystem" ] = new IlwisObjectCreatorModel("geographiccoordinatesystem", TR("LatLon Coordinate System"),itCONVENTIONALCOORDSYSTEM|itLOCATION,"CreateLatLonCoordinateSystem.qml", 290, this);
    _creators["boundsonlycoordinatesystem" ] = new IlwisObjectCreatorModel("boundsonlycoordinatesystem", TR("Bounds only Coordinate System"),itBOUNDSONLYCSY,"CreateNumDom.qml", 200, this);
    _creators["rastercoverage" ] = new IlwisObjectCreatorModel("rastercoverage", TR("Raster Coverage"),itRASTER,"CreateRasterCoverage.qml", 290, this);
    _creators["featurecoverage" ] = new IlwisObjectCreatorModel("featurecoverage", TR("Feature Coverage"),itFEATURE,"CreateNumDom.qml", 200, this);
    _creators["table" ] = new IlwisObjectCreatorModel("table", TR("Table"),itTABLE,"CreateNumDom.qml", 200, this);
    _creators["representation" ] = new IlwisObjectCreatorModel("representation",TR("Representation"),itREPRESENTATION,"CreateNumDom.qml", 250, this);
}

ObjectCreator::~ObjectCreator()
{

}

QStringList ObjectCreator::createableObjects() const
{
    QStringList nameiconpairs;
    for(auto kvp : _creators)
        nameiconpairs.append(kvp.second->label() + "|" +
                             kvp.second->name() + "|" +
                             ResourceModel::iconPath(kvp.second->type().toULongLong()));

    return nameiconpairs;
}



QString ObjectCreator::createItemDomain(const QVariantMap &parms){
    QString expression;
    if( parms["valuetype"].toString() == "thematic"){
        if ( parms["name"].toString() == ""){
            kernel()->issues()->log(TR("Domain must have a valid name"));
            return QString::number(i64UNDEF);
        }
        expression = QString("script %1{format(stream,\"domain\")}=createthematicdomain(\"%2\",%3,%4")
                .arg(parms["name"].toString())
                .arg(parms["items"].toString())
                .arg(parms["strict"].toBool() ? "yes" : "no")
                .arg(parms["description"].toString());
        if ( parms["parentdomain"].toString() != "")
            expression += ","+ parms["parentdomain"].toString();
        expression += ")";

    }
    if( parms["valuetype"].toString() == "identifier"){
        if ( parms["name"].toString() == ""){
            kernel()->issues()->log(TR("Domain must have a valid name"));
            return sUNDEF;
        }
        expression = QString("script %1{format(stream,\"domain\")}=createidentifierdomain(\"%2\",%3,%4")
                .arg(parms["name"].toString())
                .arg(parms["items"].toString())
                .arg(parms["strict"].toBool() ? "yes" : "no")
                .arg(parms["description"].toString());
        if ( parms["parentdomain"].toString() != "")
            expression += ","+ parms["parentdomain"].toString();
        expression += ")";
    }
    if( parms["valuetype"].toString() == "palette"){
        if ( parms["name"].toString() == ""){
            kernel()->issues()->log(TR("Domain must have a valid name"));
            return sUNDEF;
        }
        expression = QString("script %1{format(stream,\"domain\")}=createpalettedomain(\"%2\",%3,%4")
                .arg(parms["name"].toString())
                .arg(parms["items"].toString())
                .arg(parms["strict"].toBool() ? "yes" : "no")
                .arg(parms["description"].toString());
        if ( parms["parentdomain"].toString() != "")
            expression += ","+ parms["parentdomain"].toString();
        expression += ")";
    }
    if( parms["valuetype"].toString() == "interval"){
        if ( parms["name"].toString() == ""){
            kernel()->issues()->log(TR("Domain must have a valid name"));
            return sUNDEF;
        }
        expression = QString("script %1{format(stream,\"domain\")}=createintervaldomain(\"%2\",%3,%4,%5")
                .arg(parms["name"].toString())
                .arg(parms["items"].toString())
                .arg(parms["resolution"].toDouble())
                .arg(parms["strict"].toBool() ? "yes" : "no")
                .arg(parms["description"].toString());
        if ( parms["parentdomain"].toString() != "")
            expression += ","+ parms["parentdomain"].toString();
        expression += ")";
    }

    Ilwis::ExecutionContext ctx;
    Ilwis::SymbolTable syms;
    if(expression != "" && Ilwis::commandhandler()->execute(expression,&ctx,syms) ) {
        IDomain obj = syms.getSymbol(ctx._results[0])._var.value<IDomain>();
        return QString::number(obj->id());
    }
    return sUNDEF;
}

QString ObjectCreator::createNumericDomain(const QVariantMap &parms)
{
    if ( parms["name"].toString() == ""){
        kernel()->issues()->log(TR("Domain must have a valid name"));
        return QString::number(i64UNDEF);
    }

    QString expression = QString("script %1{format(stream,\"domain\")}=createnumericdomain(%2,%3,%4,%5,%6")
            .arg(parms["name"].toString())
            .arg(parms["minvalue"].toDouble())
            .arg(parms["maxvalue"].toDouble())
            .arg(parms["resolutionvalue"].toDouble())
            .arg(parms["strict"].toBool() ? "yes" : "no")
            .arg(parms["description"].toString());
    if ( parms["parentdomain"].toString() != "")
        expression += ","+ parms["parentdomain"].toString();
    expression += ")";

    Ilwis::ExecutionContext ctx;
    Ilwis::SymbolTable syms;
    if(Ilwis::commandhandler()->execute(expression,&ctx,syms) ) {
        IDomain obj = syms.getSymbol(ctx._results[0])._var.value<IDomain>();
        return QString::number(obj->id());
    }
    return sUNDEF;
}

QString ObjectCreator::createGeoreference(const QVariantMap &parms){
    QString expression;
    if ( parms["subtype"].toString() == "corners")  {
        expression = QString("script %1{format(stream,\"georeference\")}=createcornersgeoreference(%2,%3,%4,%5,%6,%7,%8,%9)").arg(parms["name"].toString())
                .arg(parms["minx"].toDouble())
                .arg(parms["miny"].toDouble())
                .arg(parms["maxx"].toDouble())
                .arg(parms["maxy"].toDouble())
                .arg(parms["pixelsize"].toDouble())
                .arg(parms["csy"].toString())
                .arg(parms["centered"].toString())
                .arg(parms["description"].toString());
    }
    Ilwis::ExecutionContext ctx;
    Ilwis::SymbolTable syms;
    if(Ilwis::commandhandler()->execute(expression,&ctx,syms) ) {
        IGeoReference obj = syms.getSymbol(ctx._results[0])._var.value<IGeoReference>();
        return QString::number(obj->id());
    }
    return sUNDEF;
}

QString ObjectCreator::createProjectedCoordinateSystemFromCode(const QVariantMap &parms){
    QString expression;
    if ( parms.contains("epsg")){
        expression = QString("script %1{format(stream,\"coordinatesystem\")}=createprojectedcoordinatesystem(%2)").
                arg(parms["name"].toString()).
                arg(parms["epsg"].toString());

    }
    if ( parms.contains("proj4")){
        expression = QString("script %1{format(stream,\"coordinatesystem\")}=createprojectedcoordinatesystem(\"%2\")").
                arg(parms["name"].toString()).
                arg(parms["proj4"].toString());

    }
    return expression;
}

QString ObjectCreator::createProjectedCoordinateSystemFromBase(const QVariantMap &parms){
    QString expression;
    //createprojectedcoordinatesystem(projectionname,falseeasting,falsenorthing,centralmeridian,latitudeoforigin,standardparallel1,standardparallel2,latitudeoftruescale,scale,zone,height,northoriented,azimtruescale,ellipsoid[,description])");
    QString proj = parms["projection"].toString();
    IProjection projection;
    if ( proj.indexOf("ilwis:/") != -1){
        projection.prepare(proj);
    }else {
        InternalDatabaseConnection db("Select code from projection where name='" + proj + "'");
        if ( db.next()){
            QString code = db.value(0).toString();
            projection.prepare("ilwis://tables/projection?code=" + code);
        }
    }
    if ( !projection.isValid()){
        return sUNDEF;
    }

    QString code = projection->code();
    InternalDatabaseConnection db("Select parameters from projection where code='" + code + "'");
    if (!db.next()){
        return sUNDEF;
    }
    QString kvps;

    QVariantList currentParms = parms["projectionparameters"].value<QVariantList>();

    if ( currentParms.size() > 0){
        QStringList parameterNameList = projection->parameterNameList();

        int currentIndex = 0;
        for(auto p : parameterNameList){
            if ( kvps != "")
                kvps += ",";
            kvps  += p + "= " + currentParms[currentIndex++].toString();
        }
    }
    expression = QString("script %1{format(stream,\"coordinatesystem\")}=createprojectedcoordinatesystem(\"%2\",\"%3\",\"%4\"").
            arg(parms["name"].toString()).
            arg(proj).
            arg(kvps).
            arg(parms["ellipsoid"].toString());
    if ( parms.contains("envelope")){
        expression += ",\"" + parms["envelope"].toString()+ "\"";
    }
    if ( parms.contains("datumshifts")){
        expression += ",\"" + parms["datumshifts"].toString() + "\"";
    }
    expression += ")";

    return expression;
}

QString ObjectCreator::createProjectedCoordinateSystem(const QVariantMap &parms){
    QString expression;
    if ( parms.size() == 4){
        expression =  createProjectedCoordinateSystemFromCode(parms);
    }else
        expression = createProjectedCoordinateSystemFromBase(parms);


    Ilwis::ExecutionContext ctx;
    Ilwis::SymbolTable syms;
    if(Ilwis::commandhandler()->execute(expression,&ctx,syms) ) {
        if ( ctx._results.size() > 0){
            ICoordinateSystem obj = syms.getSymbol(ctx._results[0])._var.value<ICoordinateSystem>();
            return QString::number(obj->id());
        }
    }

    return sUNDEF;
}

QString ObjectCreator::createWorkflow(const QVariantMap &parms)
{
    QString name = parms["name"].toString();
    OperationResource res(QUrl("ilwis://operations/" + name), itWORKFLOW);
    res.setDescription(parms["description"].toString());
    res.setKeywords(parms["keywords"].toString());
    res.setUrl(parms["url"].toString(),true);
    res.prepare();
    mastercatalog()->addItems({res});
    QVariant mastercatalog = uicontext()->rootContext()->contextProperty("mastercatalog");
    if ( mastercatalog.isValid()){
        MasterCatalogModel *mcmodel = mastercatalog.value<MasterCatalogModel*>();
        CatalogModel *ocmodel = mcmodel->currentCatalog();
        if ( dynamic_cast<OperationCatalogModel *>(ocmodel)){
            ocmodel->refresh();
        }
    }
    return QString::number(res.id());
}

QString ObjectCreator::createObject(const QVariantMap &parms)
{
    try {
    QString type = parms["type"].toString();
    if (  type == "workflow" ){
        return createWorkflow(parms);
    } else     if ( type == "numericdomain"){
        return createNumericDomain(parms);
    } else if ( type == "itemdomain"){
            return createItemDomain(parms);
    } else if ( type == "georef"){
        return createGeoreference(parms);
    } else if ( type == "coordinatesystem"){
        if ( parms["subtype"].toString() == "conventional")
            return createProjectedCoordinateSystem(parms);
    } else if ( type == "rastercoverage"){
        return createRasterCoverage(parms);
    }



    return QString::number(i64UNDEF);
    } catch (const ErrorObject& ){

    } catch (std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
    return sUNDEF;
}

QVariantMap ObjectCreator::creatorInfo(const QString& name) const
{
    auto iter = _creators.find(name);
    if ( iter != _creators.end()){
        QVariantMap data;
        data["name"] = (*iter).first;
        data["type"] = (*iter).second->type();
        data["label"] = (*iter).second->label();
        data["componentUrl"] = (*iter).second->componentUrl();
        data["itemHeight"] = (*iter).second->height();
        return data;
    }
    return QVariantMap();
}

QString ObjectCreator::createRasterCoverage(const QVariantMap& parms){
    QString name = parms["name"].toString();
    if ( name == "")
        return sUNDEF;

    int bands = parms["bands"].toInt();;
    QString expr = "createrastercoverage(";
    expr += parms["georeference"].toString();
    expr += ",";
    expr += parms["domain"].toString();
    expr += ",";
    expr += QString::number(bands);
    expr += ",";
    expr += "\""+ parms["description"].toString() + "\"";
    if ( parms.contains("keywords"))
        expr += ",\""+ parms["keywords"].toString() + "\"";
    expr += ")";

    QString output = QString("script %1{format(stream,\"rastercoverage\")}=").arg(name);
    expr = output + expr;
    Ilwis::ExecutionContext ctx;
    Ilwis::SymbolTable syms;
    if(Ilwis::commandhandler()->execute(expr,&ctx,syms) ) {
        if ( ctx._results.size() > 0){
            IIlwisObject obj = syms.getSymbol(ctx._results[0])._var.value<IIlwisObject>();
            if ( obj.isValid())
                return QString::number(obj->id());
        }
    }
    return sUNDEF;
}






