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
#include "workflownode.h"
#include "workflow.h"
#include "boundsonlycoordinatesystem.h"
#include "commandhandler.h"
#include "uicontextmodel.h"
#include "mastercatalogmodel.h"
#include "operationcatalogmodel.h"
#include "objectcreator.h"
#include "script.h"
#include "operationmetadata.h"
#include "analysispattern.h"
#include "applicationmodel.h"
#include "model.h"
#include "modellerfactory.h"
#include "combinationmatrix.h"
#include "workflow/modelbuilder.h"
#include "workflow/analysismodel.h"
#include "operationworker.h"

using namespace Ilwis;

ObjectCreator::ObjectCreator(QObject *parent) : QObject(parent)
{
    _creators["workflow"] = new IlwisObjectCreatorModel("workflow",TR("Workflow"),itWORKFLOW,"CreateWorkflow.qml", 400, this);
    _creators["model"] = new IlwisObjectCreatorModel("model",TR("Model"),itMODEL,"CreateModel.qml", 400, this);
    _creators["script"] = new IlwisObjectCreatorModel("script",TR("Script"),itSCRIPT,"CreateScript.qml", 700, this);
    _creators["numericdomain" ] = new IlwisObjectCreatorModel("numericdomain",TR("Numeric Domain"),itNUMERICDOMAIN,"CreateNumDom.qml", 250, this);
    _creators["thematicdomain" ] = new IlwisObjectCreatorModel("thematicdomain", TR("Thematic Domain"),itITEMDOMAIN | itTHEMATICITEM,"CreateThematicDom.qml", 520, this);
    _creators["nameidentifierdomain" ] = new IlwisObjectCreatorModel("nameidentifierdomain",TR("Identifier Domain"),itITEMDOMAIN | itIDENTIFIERITEM,"CreateIdentifierDomain.qml", 520, this);
    //_creators[ ] = new IlwisObjectCreatorModel(TR("Indexed Domain",itITEMDOMAIN | itINDEXEDITEM,"CreateNumDom.qml", 200, this);
    _creators["intervaldomain" ] = new IlwisObjectCreatorModel("intervaldomain",TR("Interval Domain"),itITEMDOMAIN | itNUMERICITEM,"CreateIntervalDomain.qml", 610, this);
    _creators["timedomain" ] = new IlwisObjectCreatorModel("timedomain",TR("Time Domain"),itTIME | itDOMAIN,"UnderDevelopment.qml", 200, this);
    _creators["timeintervaldomain" ] = new IlwisObjectCreatorModel("timeintervaldomain", TR("Time Interval Domain"),itTIMEITEM | itITEMDOMAIN,"UnderDevelopment.qml", 200, this);
//    _creators[ ] = new IlwisObjectCreatorModel(TR("Color Domain",itCOLORDOMAIN,"CreateNumDom.qml", 200, this);
    _creators["colorpalette" ] = new IlwisObjectCreatorModel("colorpalette", TR("Color Palette Domain"),itPALETTECOLOR | itITEMDOMAIN,"CreatePaletteDomain.qml", 630, this);
    _creators["cornersgeoreferences" ] = new IlwisObjectCreatorModel("cornersgeoreferences", TR("Corners Georeference"),itGEOREF,"CreateGeorefCorners.qml", 330, this);
    _creators["tiepointgeoreference" ] = new IlwisObjectCreatorModel("tiepointgeoreference",TR("Tiepoints Georeference"),itGEOREF | itLOCATION,"CreateGeorefTiepoints.qml", 280, this);
    _creators["projectedcoordinatesystem" ] = new IlwisObjectCreatorModel("projectedcoordinatesystem", TR("Projected Coordinate System"),itCONVENTIONALCOORDSYSTEM,"CreateProjectedCoordinateSystem.qml", 530, this);
    _creators["geographiccoordinatesystem" ] = new IlwisObjectCreatorModel("geographiccoordinatesystem", TR("Geographic (LatLon) Coordinate System"),itCONVENTIONALCOORDSYSTEM|itLOCATION,"CreateLatLonCoordinateSystem.qml", 320, this);
    _creators["boundsonlycoordinatesystem" ] = new IlwisObjectCreatorModel("boundsonlycoordinatesystem", TR("Bounds only Coordinate System"),itBOUNDSONLYCSY,"CreateBoundsOnlyCsy.qml", 250, this);
    _creators["rastercoverage" ] = new IlwisObjectCreatorModel("rastercoverage", TR("Raster Coverage"),itRASTER,"CreateRasterCoverage.qml", 390, this);
    _creators["featurecoverage" ] = new IlwisObjectCreatorModel("featurecoverage", TR("Feature Coverage"),itFEATURE,"UnderDevelopment.qml", 200, this);
    _creators["table" ] = new IlwisObjectCreatorModel("table", TR("Table"),itTABLE,"UnderDevelopment.qml", 200, this);
    _creators["representation" ] = new IlwisObjectCreatorModel("representation",TR("Representation"),itREPRESENTATION,"UnderDevelopment.qml", 250, this);
    _creators["domain" ] = new IlwisObjectCreatorModel("domain",TR("Domain"),itDOMAIN,"CreateDomain.qml", 250, this);
    _creators["combinationmatrix" ] = new IlwisObjectCreatorModel("combinationmatrix",TR("Combinationmatrix"),itCOMBINATIONMATRIX,"CreateCombinationMatrix.qml", 600, this);
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


QString ObjectCreator::createCombinationMatrix(const QVariantMap &parms){
    QString expression;
    expression = QString("script %1{format(stream,\"combinationmatrix\")}=createcombinationmatrix(%2,%3,%4,\"%5\",\"%6\",\"%7\",\"%8\")")
            .arg(parms["name"].toString())
            .arg(parms["xaxisdomain"].toString())
            .arg(parms["yaxisdomain"].toString())
            .arg(parms["combodomain"].toString())
            .arg(parms["xaxis"].toString())
            .arg(parms["yaxis"].toString())
            .arg(parms["combinations"].toString())
            .arg(parms["description"].toString());

    Ilwis::ExecutionContext ctx;
    Ilwis::SymbolTable syms;
    if(expression != "" && Ilwis::commandhandler()->execute(expression,&ctx,syms) ) {
        ICombinationMatrix obj = syms.getSymbol(ctx._results[0])._var.value<ICombinationMatrix>();
        return QString::number(obj->id());
    }
    return sUNDEF;
}

QString ObjectCreator::createItemDomain(const QVariantMap &parms){
    QString expression;
    if( parms["valuetype"].toString() == "thematic"){
        if ( parms["name"].toString() == ""){
            kernel()->issues()->log(TR("Domain must have a valid name"));
            return QString::number(i64UNDEF);
        }
        expression = QString("script %1{format(stream,\"domain\")}=createthematicdomain(\"%2\",%3,\"%4\"")
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
        expression = QString("script %1{format(stream,\"domain\")}=createidentifierdomain(\"%2\",%3,\"%4\"")
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
        expression = QString("script %1{format(stream,\"domain\")}=createpalettedomain(\"%2\",%3,\"%4\"")
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
        expression = QString("script %1{format(stream,\"domain\")}=createintervaldomain(\"%2\",%3,%4,\"%5\"")
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

    QString expression = QString("script %1{format(stream,\"domain\")}=createnumericdomain(%2,%3,%4,%5,\"%6\"")
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

QString ObjectCreator::createBoundsOnlyCoordinateSystem(const QVariantMap &parms){
    QString expression;
    expression = QString("script %1{format(stream,\"coordinatesystem\")}=createboundsonlycsy(%2,%3,%4,%5,\"%6\")").arg(parms["name"].toString())
            .arg(parms["minx"].toDouble())
            .arg(parms["miny"].toDouble())
            .arg(parms["maxx"].toDouble())
            .arg(parms["maxy"].toDouble())
            .arg(parms["description"].toString());
    Ilwis::ExecutionContext ctx;
    Ilwis::SymbolTable syms;
    if(Ilwis::commandhandler()->execute(expression,&ctx,syms) ) {
        if ( ctx._results.size() > 0){
            IBoundsOnlyCoordinateSystem obj = syms.getSymbol(ctx._results[0])._var.value<IBoundsOnlyCoordinateSystem>();
            return QString::number(obj->id());
        }
    }
    return sUNDEF;
}

QString ObjectCreator::createGeoreference(const QVariantMap &parms){
    QString expression;
    if ( parms["subtype"].toString() == "corners")  {
        expression = QString("script %1{format(stream,\"georeference\")}=createcornersgeoreference(%2,%3,%4,%5,%6,%7,%8,\"%9\")").arg(parms["name"].toString())
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
        if ( ctx._results.size() > 0){
            IGeoReference obj = syms.getSymbol(ctx._results[0])._var.value<IGeoReference>();
            return QString::number(obj->id());
        }
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
            projection.prepare("ilwis://system/projections/" + code);
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
    IWorkflow wf;
    wf.prepare();
    QString name = parms["name"].toString();
    wf->resourceRef().name(name,false,false);
    wf->resourceRef().setUrl(QUrl("ilwis://operations/" + name + ".ilwis"));
    wf->resourceRef().setUrl(parms["url"].toString(), true);
    wf->resourceRef().setDescription(parms["description"].toString());
    wf->resourceRef().addProperty("keyword", parms["keywords"].toString());
    wf->resourceRef().addProperty("longname", parms["longname"].toString());
    wf->connectTo(parms["url"].toString(),"workflow","stream",IlwisObject::cmOUTPUT);
    wf->store();
    mastercatalog()->addItems({wf->resource()});
    QVariant mastercatalog = uicontext()->rootContext()->contextProperty("mastercatalog");
    if ( mastercatalog.isValid()){
        MasterCatalogModel *mcmodel = mastercatalog.value<MasterCatalogModel*>();
        CatalogModel *ocmodel = mcmodel->currentCatalog();
        if ( dynamic_cast<OperationCatalogModel *>(ocmodel)){
            ocmodel->refresh();
        }
    }
    return QString::number(wf->id());
}
QString ObjectCreator::createOperationScriptHeader(const QVariantMap& parms){
    QString header = "#- ILWIS OBJECTS OPERATION METADATA\n";
    QString operationname = parms["operationname"].toString();
    operationname = operationname.replace(QRegExp("[/ .-,;:'\"]"),"_");
    header += "#- operation = ilwis://operations/" + operationname + "\n";
    QString longname = parms["longname"].toString();
    if ( longname != sUNDEF && longname != ""){
        header += "#- longname = " + longname + "\n";
    }
    int inparmCount = parms["inputparameters"].toList().size();
    header += "#- input parameter count = " + QString::number(inparmCount) + "\n";
    QString syntax = operationname + "(";
    for(int i = 0; i < inparmCount; ++i){
        QVariantMap props = parms["inputparameters"].toList()[i].toMap();
        QString name = props["name"].toString();
        QString parm = props["valuetype"].toString() + "|" + name + "|" + props["description"].toString();
        header += QString("#- input parameter %1 = %2\n").arg(i+1).arg(parm) ;
        name  =name.replace(QRegExp("[/ .-,;:'\"]"),"_");
        if ( i != 0)
               syntax += ",";
        syntax += name;
    }
    syntax += ")\n";
    header += "#- syntax = " + syntax;
    int outparmCount = parms["outputparameters"].toList().size();
    header += "#- output parameter count = " + QString::number(outparmCount) + "\n";
    for(int i = 0; i < outparmCount; ++i){
        QVariantMap props = parms["outputparameters"].toList()[i].toMap();
        QString parm = props["valuetype"].toString() + "|" + props["name"].toString() + "|" + props["description"].toString();
        header += QString("#- output parameter %1 = %2\n").arg(i+1).arg(parm) ;
    }
    header += "#- keywords = " + parms["keywords"].toString() + "\n";
    header += "#- description = " + parms["description"].toString() + "\n";
    header += "#- END ILWIS OBJECTS OPERATION METADATA\n";
    header += "import ilwis\n";
    header += "def " + operationname + "(";
    for(int i=0; i < inparmCount; ++i){
        if ( i!= 0){
            header += ",";
        }
        QVariantMap props = parms["inputparameters"].toList()[i].toMap();
        QString name = props["name"].toString();
        name = name.replace(QRegExp("[/ .'\"-]"),"_");
        header += name;
    }
    header += "):\n";

    return header;
}

OperationResource ObjectCreator::createOperationResource(const QString& url, const QVariantMap& parms){
    QString operationname = parms["operationname"].toString();
    if ( operationname == ""){
        kernel()->issues()->log(TR("An operation must have a name"));
        return OperationResource();
    }
    operationname = operationname.replace(QRegExp("[/ .-,;:'\"]"),"_");
    OperationResource opResource("ilwis://operations/" + operationname,"python");
    opResource.setExtendedType(itSCRIPT);
    QString longname = parms["longname"].toString();
    if ( longname != sUNDEF && longname != "")
        opResource.setLongName(longname);
    opResource.setDescription(parms["description"].toString() );
    int inparmCount = parms["inputparameters"].toList().size();
    opResource.setInParameterCount({inparmCount});
    QString syntax = operationname + "(";
    for(int i=0; i < inparmCount; ++i){
         QVariantMap props = parms["inputparameters"].toList()[i].toMap();
        IlwisTypes tp = TypeHelper::name2type(props["valuetype"].toString());
        QString name = props["name"].toString();
        if ( name == ""){
            kernel()->issues()->log(TR("A parameter must have a name"));
            return OperationResource();
        }
        QString description = props["description"].toString();
        opResource.addInParameter(i, tp, name, description);
        name  =name.replace(QRegExp("[/ .-,;:'\"]"),"_");
        if ( i != 0)
            syntax += ",";
        syntax += name;
    }
    syntax += ")";
    opResource.setSyntax(syntax);
    int outparmCount = parms["outputparameters"].toList().size();
    opResource.setOutParameterCount({outparmCount});
    for(int i=0; i < outparmCount; ++i){
         QVariantMap props = parms["outputparameters"].toList()[i].toMap();
        IlwisTypes tp = TypeHelper::name2type(props["valuetype"].toString());
        QString name = props["name"].toString();
        if ( name == ""){
            kernel()->issues()->log(TR("A parameter must have a name"));
            return OperationResource();
        }
        QString description = props["description"].toString();
        opResource.addOutParameter(i, tp, name, description);
    }
    quint64 id = mastercatalog()->name2id("ilwis://operations/callpython", itSINGLEOPERATION);
    if (id == i64UNDEF){
        kernel()->issues()->log(TR("Python module is not properly loaded by ilwis"));
        return OperationResource();
    }
    opResource.addProperty("stuboperation", id);
    opResource.setKeywords(parms["keywords"].toString());
    opResource.setUrl(url, true);

    return opResource;

}

QString ObjectCreator::createScript(const QVariantMap &parms)
{
    Ilwis::IScript script;
    script.prepare();
    QString name = parms["name"].toString();
    script->name(name);
    QString url = parms["url"].toString();
    if ( url.indexOf(".py") == -1){
        url += ".py";
    }
    if ( parms.contains("asoperation") && parms["asoperation"].toBool()){
        OperationResource ores = createOperationResource(url, parms);
        if (!ores.isValid()){
            kernel()->issues()->log(TR("Invalid operation metadata"));
            return QString::number(i64UNDEF);
        }
        if ( ores.compatibleOperationAlreadyExists()){
            kernel()->issues()->log(TR("An operation with the same signature already exists. Change the name or parameter order/types"));
            return QString::number(i64UNDEF);
        }
        script->resourceRef() = ores;
    }else{
        script->resourceRef().setUrl(url);
        script->resourceRef().setUrl(url, true);
        script->resourceRef().setDescription(parms["description"].toString());
        script->resourceRef().addProperty("keyword", parms["keywords"].toString());
        script->connectTo(parms["url"].toString(),"script","python",IlwisObject::cmOUTPUT);
    }

    if ( parms.contains("asoperation") && parms["asoperation"].toBool()){
        QString header = createOperationScriptHeader( parms);
        QString filename = QUrl(url).toLocalFile();
        QFile file(filename);
        if (file.open(QFile::WriteOnly | QFile::Text)){
            file.write(header.toLocal8Bit());
            file.close();
        }
    }

    mastercatalog()->addItems({script->resource()});
    QVariant masterc = uicontext()->rootContext()->contextProperty("mastercatalog");
    if ( masterc.isValid()){
        MasterCatalogModel *mcmodel = masterc.value<MasterCatalogModel*>();
        CatalogModel *ocmodel = mcmodel->currentCatalog();
        if ( dynamic_cast<OperationCatalogModel *>(ocmodel)){
            ocmodel->refresh();
        }
    }
    return QString::number(script->id());
}

QString ObjectCreator::createModel(const QVariantMap &parms){
    IModel model;
    model.prepare();
    QString name = parms["name"].toString();
    model->resourceRef().name(name,false,false);
    model->resourceRef().setUrl(parms["url"].toString());
    model->resourceRef().setUrl(parms["url"].toString(), true);
    model->resourceRef().setDescription(parms["description"].toString());
    model->resourceRef().addProperty("keyword", parms["keywords"].toString());
    model->connectTo(parms["url"].toString(),"model","stream",IlwisObject::cmOUTPUT);
    model->store();
    mastercatalog()->addItems({model->resource()});
    QVariant mastercatalog = uicontext()->rootContext()->contextProperty("mastercatalog");
    if ( mastercatalog.isValid()){
        MasterCatalogModel *mcmodel = mastercatalog.value<MasterCatalogModel*>();
        CatalogModel *ocmodel = mcmodel->currentCatalog();
        if ( dynamic_cast<OperationCatalogModel *>(ocmodel)){
            ocmodel->refresh();
        }
    }
    return QString::number(model->id());
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
        if ( parms["subtype"].toString() == "boundsonly")
            return createBoundsOnlyCoordinateSystem(parms);
    } else if ( type == "rastercoverage"){
        return createRasterCoverage(parms);
    }else if ( type == "script"){
        return createScript(parms);
    }else if ( type == "model"){
        return createModel(parms);
    } else if ( type == "combinationmatrix"){
        return createCombinationMatrix(parms);
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

QObject *ObjectCreator::createModellerObject(const QVariantMap &parms, QObject *parent)
{
    QString modellerObjectType = parms["type"].toString();
    if ( modellerObjectType == "analysispattern"){
        bool ok;
        quint64 modelId = parms["modelId"].toULongLong(&ok);
        if ( ok){
            IModel currentModel;
            if ( currentModel.prepare(modelId)){
                QString subtype = parms["subtype"].toString();
                QString patternName = parms["pattername"].toString();
                if ( patternName != ""){
                    QString desc= parms["description"].toString();
                    ModellerFactory *factory = kernel()->factory<ModellerFactory>("ModellerFactory","ilwis");
                    AnalysisPattern * pattern = factory->createAnalysisPattern(subtype, patternName, desc, IOOptions());
                    if ( pattern){
                        currentModel->addAnalysisPattern(pattern);
                        AnalysisModel *amodel = modelbuilder()->createAnalysisModel(pattern);
                        return amodel; // the ownership of this model is in the modeldesigner to which this analysis will be added
                    }
                }else
                    kernel()->issues()->log(TR("An analysis must have a name"), IssueObject::itWarning);
            }
        }
    } else    if ( modellerObjectType == "applicationmodel"){
        bool ok;
        quint64 modelId = parms["modelId"].toULongLong(&ok);
        if ( ok){
            IModel currentModel;
            if ( currentModel.prepare(modelId)){
                QString subtype = parms["subtype"].toString();

                ModellerFactory *factory = kernel()->factory<ModellerFactory>("ModellerFactory","ilwis");
                ApplicationModel * app = factory->createApplication(subtype);
                if ( app){
                    currentModel->addApplication(app);
                    ApplicationModelUI *amodel = modelbuilder()->createApplicationModelUI(app, 0);
                    return (QObject *)amodel; // the ownership of this model is in the modeldesigner to which this analysis will be added

                }else
                    kernel()->issues()->log(TR("An analysis must have a name"), IssueObject::itWarning);
            }
        }
    }
    return 0;
}

QString ObjectCreator::createRasterCoverage(const QVariantMap& parms){
    QString name = parms["name"].toString();
    if ( name == "")
        return sUNDEF;

    QString expr = "createrastercoverage(";
    expr += parms["georeference"].toString();
    expr += ",";
    expr += parms["domain"].toString();
    expr += ",";
    expr += "\"" + parms["bands"].toString() + "\"";
    expr += ",";
    expr += parms["stackdomain"].toString();
    expr += ",";
    expr += "\"" + parms["stackdefinition"].toString() + "\"";
    expr += ",";
    expr += parms["autoresample"].toBool() ? "yes" : "no";


    expr += ")";

    QString output = QString("script %1{format(stream,\"rastercoverage\")}=").arg(name);
    expr = output + expr;
    executeoperation(expr);
//    Ilwis::ExecutionContext ctx;
//    Ilwis::SymbolTable syms;
//    if(Ilwis::commandhandler()->execute(expr,&ctx,syms) ) {
//        if ( ctx._results.size() > 0){
//            IIlwisObject obj = syms.getSymbol(ctx._results[0])._var.value<IIlwisObject>();
//            if ( obj.isValid())
//                return QString::number(obj->id());
//        }
//    }
    return sUNDEF;
}

void ObjectCreator::executeoperation(const QString& expr) {

    try {
        OperationExpression opExpr(expr);
        if (opExpr.isValid()){

            QThread* thread = new QThread;
            OperationWorker* worker = new OperationWorker(opExpr);
            worker->moveToThread(thread);
            thread->setProperty("workingcatalog", qVariantFromValue(context()->workingCatalog()));
            thread->connect(thread, &QThread::started, worker, &OperationWorker::process);
            thread->connect(worker, &OperationWorker::finished, thread, &QThread::quit);
            thread->connect(worker, &OperationWorker::finished, worker, &OperationWorker::deleteLater);
            thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
            thread->start();


        }
    } catch (const ErrorObject& err){
        emit error(err.message());
    }
}





