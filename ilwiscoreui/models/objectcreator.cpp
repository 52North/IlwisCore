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
    _creators.append(new IlwisObjectCreatorModel("Workflow",itWORKFLOW,"CreateWorkflow.qml", 400, this));
    _creators.append(new IlwisObjectCreatorModel("Numeric Domain",itNUMERICDOMAIN,"CreateNumDom.qml", 250, this));
    _creators.append(new IlwisObjectCreatorModel("Thematic Domain",itITEMDOMAIN | itTHEMATICITEM,"CreateThematicDom.qml", 520, this));
    _creators.append(new IlwisObjectCreatorModel("Identifier Domain",itITEMDOMAIN | itIDENTIFIERITEM,"CreateIdentifierDomain.qml", 520, this));
    //_creators.append(new IlwisObjectCreatorModel("Indexed Domain",itITEMDOMAIN | itINDEXEDITEM,"CreateNumDom.qml", 200, this));
    _creators.append(new IlwisObjectCreatorModel("Interval Domain",itITEMDOMAIN | itNUMERICITEM,"CreateIntervalDomain.qml", 550, this));
    _creators.append(new IlwisObjectCreatorModel("Time Domain",itTIME | itDOMAIN,"CreateNumDom.qml", 200, this));
    _creators.append(new IlwisObjectCreatorModel("Time Interval Domain",itTIMEITEM | itITEMDOMAIN,"CreateNumDom.qml", 200, this));
//    _creators.append(new IlwisObjectCreatorModel("Color Domain",itCOLORDOMAIN,"CreateNumDom.qml", 200, this));
    _creators.append(new IlwisObjectCreatorModel("Color Palette Domain",itPALETTECOLOR | itITEMDOMAIN,"CreatePaletteDomain.qml", 560, this));
    _creators.append(new IlwisObjectCreatorModel("Corners Georeference",itGEOREF,"CreateGeorefCorners.qml", 350, this));
    _creators.append(new IlwisObjectCreatorModel("Tiepoints Georeference",itGEOREF | itLOCATION,"CreateGeorefTiepoints.qml", 280, this));
    _creators.append(new IlwisObjectCreatorModel("Projected Coordinate System",itCONVENTIONALCOORDSYSTEM,"CreateProjectedCoordinateSystem.qml", 500, this));
    _creators.append(new IlwisObjectCreatorModel("LatLon Coordinate System",itCONVENTIONALCOORDSYSTEM|itLOCATION,"CreateNumDom.qml", 200, this));
    _creators.append(new IlwisObjectCreatorModel("Bounds only Coordinate System",itBOUNDSONLYCSY,"CreateNumDom.qml", 200, this));
    _creators.append(new IlwisObjectCreatorModel("Raster Coverage",itRASTER,"CreateNumDom.qml", 200, this));
    _creators.append(new IlwisObjectCreatorModel("Feature Coverage",itFEATURE,"CreateNumDom.qml", 200, this));
    _creators.append(new IlwisObjectCreatorModel("Table",itTABLE,"CreateNumDom.qml", 200, this));
    _creators.append(new IlwisObjectCreatorModel("Representation",itREPRESENTATION,"CreateNumDom.qml", 250, this));
}

ObjectCreator::~ObjectCreator()
{

}

QStringList ObjectCreator::createableObjects() const
{
    QStringList nameiconpairs;
    for(auto *model : _creators)
        nameiconpairs.append(model->label() + "|" + ResourceModel::iconPath(model->type().toULongLong()));

    return nameiconpairs;
}

void ObjectCreator::setActiveCreator(qint32 index)
{
    if ( index >= 0 && index < _creators.size()){
        _activeCreators.clear();
        _activeCreators.append(_creators[index]);
        emit activeCreatorsChanged();
    } if ( index == -1){
        _activeCreators.clear();
        emit activeCreatorsChanged();

    }
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
            return QString::number(i64UNDEF);
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
            return QString::number(i64UNDEF);
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
            return QString::number(i64UNDEF);
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
    return QString::number(i64UNDEF);
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
    return QString::number(i64UNDEF);
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
    return QString::number(i64UNDEF);
}

QString ObjectCreator::createProjectedCoordinateSystem(const QVariantMap &parms){
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
        return QString::number(i64UNDEF);
    }

    QString code = projection->code();
    InternalDatabaseConnection db("Select parameters from projection where code='" + code + "'");
    if (!db.next()){
        return QString::number(i64UNDEF);
    }
    QString parameters = db.value(0).toString();
    QVariantList currentParms = parms["projectionparameters"].value<QVariantList>();

    QStringList parmlist = parameters.split("|");
    std::vector<QString> projectionparms(12,"0");
    projectionparms[9] = "true";
    projectionparms[8] = 1;
    projectionparms[7] = 1;

    int currentIndex = 0;
    for(auto p : parmlist){
        if ( p == "false easting")
            projectionparms[0] = currentParms[currentIndex++].toString();
        else if ( p == "false northing")
            projectionparms[1] = currentParms[currentIndex++].toString();
        else if ( p == "central meridian")
            projectionparms[2] = currentParms[currentIndex++].toString();
        else if ( p == "latitude of origin")
            projectionparms[3] = currentParms[currentIndex++].toString();
        else if ( p == "latitude of true scale")
            projectionparms[4] = currentParms[currentIndex++].toString();
        else if ( p == "standard parallel 1")
            projectionparms[5] = currentParms[currentIndex++].toString();
        else if ( p == "standard parallel 2")
            projectionparms[6] = currentParms[currentIndex++].toString();
        else if ( p == "scale factor")
            projectionparms[7] = currentParms[currentIndex++].toString();
        else if ( p == "zone")
            projectionparms[8] = currentParms[currentIndex++].toString();
        else if ( p == "north oriented")
            projectionparms[9] = (currentParms[currentIndex++].toBool() ? "yes" : "no");
        else if ( p == "height")
            projectionparms[10] = currentParms[currentIndex++].toString();
        else if ( p == "azim central line of true scale")
            projectionparms[11] = currentParms[currentIndex++].toBool();

    }
    expression = QString("script %1{format(stream,\"coordinatesystem\")}=createprojectedcoordinatesystem(%2").arg(parms["name"].toString());
    for(auto p : projectionparms){
        expression += ","+ p;
    }
    expression += "," + parms["ellipsoid"].toString();
    expression += "," + parms["description"].toString() + ")";

    Ilwis::ExecutionContext ctx;
    Ilwis::SymbolTable syms;
    if(Ilwis::commandhandler()->execute(expression,&ctx,syms) ) {
        ICoordinateSystem obj = syms.getSymbol(ctx._results[0])._var.value<ICoordinateSystem>();
        return QString::number(obj->id());
    }

    return QString::number(i64UNDEF);
}

QString ObjectCreator::createWorkflow(const QVariantMap &parms)
{
    QString name = parms["name"].toString();
    OperationResource res(QUrl("ilwis://operations/" + name), itWORKFLOW);
    res.setDescription(parms["description"].toString());
    res.setKeywords(parms["keywords"].toString());
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
    Resource res;
    res.setDescription(parms["decription"].toString());

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
        if ( parms["subtype"].toString() == "projected")
            return createProjectedCoordinateSystem(parms);
    }



    return QString::number(i64UNDEF);
    } catch (const ErrorObject& ){

    } catch (std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
    return QString::number(i64UNDEF);
}

QQmlListProperty<IlwisObjectCreatorModel> ObjectCreator::activeCreators()
{
    return  QQmlListProperty<IlwisObjectCreatorModel>(this, _activeCreators);
}


