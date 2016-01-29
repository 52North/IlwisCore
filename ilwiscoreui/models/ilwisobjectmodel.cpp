#include "coverage.h"
#include "ilwisobjectmodel.h"
#include "projection.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "conventionalcoordinatesystem.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"
#include "itemrange.h"
#include "colorrange.h"
#include "operationmodel.h"
#include "catalogmodel.h"
#include "raster.h"

using namespace Ilwis;

IlwisObjectModel::IlwisObjectModel()
{
}

IlwisObjectModel::IlwisObjectModel(const Ilwis::Resource &source, QObject *parent) : ResourceModel(source, parent)
{
    try{
        if ( source.name() != "Global Layer"){ // special case for the dummy object of the global layer
            _ilwisobject.prepare(resource());
        }
    } catch (const ErrorObject& ){

    } catch ( std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
}

QString IlwisObjectModel::creationDate() const
{
    if ( _ilwisobject.isValid()){
        Time creationTime = _ilwisobject->createTime();
        if ( creationTime.isValid())
            return creationTime.toString();
    }

    return "";
}

QString IlwisObjectModel::modifiedDate() const
{
    if ( _ilwisobject.isValid()){
        Time modifiedTime = _ilwisobject->modifiedTime();
        if ( modifiedTime.isValid())
            return modifiedTime.toString();
    }

    return "";
}

bool IlwisObjectModel::readonly() const
{
    if ( _ilwisobject.isValid()){
        return _ilwisobject->isReadOnly();
    }
    return true;
}

void IlwisObjectModel::readonly(bool yesno) const
{
    if ( _ilwisobject.isValid()){
        _ilwisobject->readOnly(yesno);
    }
}

QString IlwisObjectModel::description() const
{
    if ( _ilwisobject.isValid()){
        QString desc = _ilwisobject->description();
        if ( desc != sUNDEF)
            return desc;
    }
    return "";
}

QString IlwisObjectModel::externalFormat() const
{
    if ( _ilwisobject.isValid()){
        QString inFormat = _ilwisobject->formatCode();
        QString outFormat = _ilwisobject->formatCode(false);
        QString provider = _ilwisobject->provider();
        if ( outFormat == "" && inFormat == "")
            return sUNDEF;
        if ( outFormat == inFormat)
            return provider + ": " + inFormat;
        if ( outFormat == "" && inFormat != "")
            return provider + ": " +inFormat;
        if ( inFormat == "" && outFormat != "")
            return "internal/"  + outFormat;
        return provider + ": " + inFormat + "/" + outFormat;
    }
    return "";
}

bool IlwisObjectModel::externalReadOnly() const
{
   if ( _ilwisobject.isValid()){
       return _ilwisobject->outputConnectionReadonly();
   }
   return true;
}

void IlwisObjectModel::description(const QString &desc) const
{
    if ( _ilwisobject.isValid())
        _ilwisobject->setDescription(desc);
}

bool IlwisObjectModel::isProjectedCoordinateSystem() const
{
    try {
        ICoordinateSystem csy;
        if ( hasType(_ilwisobject->ilwisType(), itCOVERAGE | itCOORDSYSTEM)){
            csy = hasType(_ilwisobject->ilwisType(), itCOVERAGE) ? _ilwisobject.as<Coverage>()->coordinateSystem() : _ilwisobject.as<CoordinateSystem>();
        } if ( hasType(_ilwisobject->ilwisType(), itGEOREF)){
            csy = _ilwisobject.as<GeoReference>()->coordinateSystem();
        }
        if ( csy.isValid()){
            if ( csy->isLatLon())
                return false;

            if (csy->ilwisType() != itCONVENTIONALCOORDSYSTEM)
                return false;

            return true;
        }

        return false;
    }catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return "";
}

bool IlwisObjectModel::isSystemObject() const
{
    if ( _ilwisobject.isValid()){
        return _ilwisobject->isSystemObject();
    }
    return true; // just block
}

bool IlwisObjectModel::isCoverage() const
{
    if ( _ilwisobject.isValid()){
        return hasType(_ilwisobject->ilwisType(), itCOVERAGE);
    }
    return false;
}

QString IlwisObjectModel::projectionInfo() const
{
    try {
        if ( isProjectedCoordinateSystem()){
            IConventionalCoordinateSystem csy = hasType(_ilwisobject->ilwisType(), itCOVERAGE) ? _ilwisobject.as<Coverage>()->coordinateSystem().as<ConventionalCoordinateSystem>()
                                                                                               : _ilwisobject.as<ConventionalCoordinateSystem>();
            QString projection = Projection::projectionCode2Name(csy->projection()->code());
            QVariant var = csy->projection()->parameter(Projection::pvZONE);
            if ( var.isValid() && !var.toInt() == 1){
                projection += " zone=" + var.toString();
            }
            QString ellipsoid = Ellipsoid::ellipsoidCode2Name(csy->ellipsoid()->name());
            if ( ellipsoid == sUNDEF){
                QString wkt = csy->ellipsoid()->wktShort();
                if ( wkt != sUNDEF && wkt != "")
                    ellipsoid = wkt;
                else {
                    QString authority = csy->ellipsoid()->authority();
                    if ( wkt != sUNDEF && wkt != "")
                        ellipsoid = authority;
                    else
                        ellipsoid = QString("custom(%1,%2)").arg(csy->ellipsoid()->majorAxis(),0,'f',3).arg(csy->ellipsoid()->flattening(),0,'f',6);
                }
            }
            return projection + "; ellipsoid=" + ellipsoid;
        }
        return "";
    } catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return "";
}

void IlwisObjectModel::resetAttributeModel(const QString& attributeName){

    auto setAttributeModel = [&](int i, const ColumnDefinition& coldef, const QString& attributeName){
        if ( coldef.name() == attributeName){
            AttributeModel *attribute = new AttributeModel(coldef, this, _ilwisobject);
            _attributes[i] = attribute;
        }
    };

    IlwisTypes objecttype = _ilwisobject->ilwisType();
    if ( objecttype == itRASTER){
        IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
        if ( raster->hasAttributes()){
            for(int i = 0; i < raster->attributeTable()->columnCount(); ++i){
                setAttributeModel(i,raster->attributeTable()->columndefinition(i), attributeName);
            }
        }
    } else if ( hasType(objecttype,itFEATURE)){
        IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
        for(int i = 0; i < features->attributeDefinitions().definitionCount(); ++i){
            setAttributeModel(i,features->attributeTable()->columndefinition(i), attributeName);
        }
    } else if ( hasType(objecttype,itTABLE)){
        ITable tbl = _ilwisobject.as<Table>();
        for(int i = 0; i < tbl->columnCount(); ++i){
            setAttributeModel(i,tbl->columndefinition(i),attributeName);
        }
    }

}

QQmlListProperty<AttributeModel> IlwisObjectModel::attributes()
{
    try {
        if ( _attributes.size() == 0){
            if ( _ilwisobject.isValid())    {
                IlwisTypes objecttype = _ilwisobject->ilwisType();
                if ( objecttype == itRASTER){
                    IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();

                    if ( raster->hasAttributes()){
                        for(int i = 0; i < raster->attributeTable()->columnCount(); ++i){
                            AttributeModel *attribute = new AttributeModel(raster->attributeTable()->columndefinition(i), this, _ilwisobject);
                            _attributes.push_back(attribute);
                        }
                    }else {
                        AttributeModel *attribute = new AttributeModel(ColumnDefinition(PIXELVALUE, raster->datadef(),i64UNDEF), this, _ilwisobject);
                        _attributes.push_back(attribute);
                    }
                } else if ( hasType(objecttype,itFEATURE)){
                    IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                    for(int i = 0; i < features->attributeDefinitions().definitionCount(); ++i){
                        AttributeModel *attribute = new AttributeModel(features->attributeDefinitions().columndefinition(i), this, _ilwisobject);
                        _attributes.push_back(attribute);
                    }
                } else if ( hasType(objecttype,itTABLE)){
                    ITable tbl = _ilwisobject.as<Table>();
                    for(int i = 0; i < tbl->columnCount(); ++i){
                        AttributeModel *attribute = new AttributeModel(tbl->columndefinition(i), this, _ilwisobject);
                        _attributes.push_back(attribute);
                    }
                }
            }
        }
        if ( _attributes.size() > 0){
            return QQmlListProperty<AttributeModel>(this, _attributes) ;
        }
    }
    catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return QQmlListProperty<AttributeModel>();

}

QQmlListProperty<DomainItemModel> IlwisObjectModel::domainitems()
{
    IlwisTypes objectype = _ilwisobject->ilwisType();
    if ( hasType( objectype, itDOMAIN)){
        IDomain domain = _ilwisobject.as<Domain>();
        if ( hasType(domain->ilwisType(), itITEMDOMAIN)){
            SPItemRange itemrange =domain->range<ItemRange>();
            if ( !itemrange.isNull()){
                for(auto item : *(itemrange.data())) {
                    if ( item){
                        DomainItemModel *domainitem = new DomainItemModel(itemrange,item->name(), this);
                        _domainItems.push_back(domainitem);
                    }
                }
                if ( _domainItems.size() > 0)
                    return QQmlListProperty<DomainItemModel>(this, _domainItems) ;
            }
        }
    }
    return QQmlListProperty<DomainItemModel>();
}

QQmlListProperty<ProjectionParameterModel> IlwisObjectModel::projectionItems()
{
    IlwisTypes objectype = _ilwisobject->ilwisType();
    if ( hasType( objectype, itPROJECTION | itCONVENTIONALCOORDSYSTEM)){
        IProjection proj;
        _projectionParmItems.clear();
        if ( hasType(objectype, itCONVENTIONALCOORDSYSTEM)){
            IConventionalCoordinateSystem csyProj = _ilwisobject.as<ConventionalCoordinateSystem>();
            if ( csyProj.isValid()){
                proj = csyProj->projection();
            }
        }else
            proj = _ilwisobject.as<Projection>();

        if ( proj.isValid()){
            if ( proj->isUsed(Projection::pvX0))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvX0, this));
            if ( proj->isUsed(Projection::pvY0))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvY0, this));
            if ( proj->isUsed(Projection::pvLON0))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvLON0, this));
            if ( proj->isUsed(Projection::pvLAT0))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvLAT0, this));
            if ( proj->isUsed(Projection::pvLAT1))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvLAT1, this));
            if ( proj->isUsed(Projection::pvLAT2))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvLAT2, this));
            if ( proj->isUsed(Projection::pvLATTS))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvLATTS, this));
            if ( proj->isUsed(Projection::pvK0))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvK0, this));
            if ( proj->isUsed(Projection::pvZONE))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvZONE, this));
            if ( proj->isUsed(Projection::pvHEIGHT))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvHEIGHT, this));
        }
        return QQmlListProperty<ProjectionParameterModel>(this, _projectionParmItems);
    }

    return QQmlListProperty<ProjectionParameterModel>();
}

QString IlwisObjectModel::valuetype() const
{
    try{
        if ( !_ilwisobject.isValid())
            return "";

        IlwisTypes objectype = _ilwisobject->ilwisType();
        IlwisTypes valueType = itUNKNOWN;
        if ( hasType( objectype, itCOVERAGE|itDOMAIN)){
            if ( objectype == itRASTER){
                IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
                valueType = raster->datadef().domain()->valueType();
            } else if ( hasType( objectype , itFEATURE)){
                IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                ColumnDefinition coldef = features->attributeDefinitions().columndefinition(COVERAGEKEYCOLUMN);
                if ( coldef.isValid()){
                    valueType = coldef.datadef().domain()->valueType();
                }

            } else if ( hasType( objectype , itDOMAIN)){
                IDomain dom = _ilwisobject.as<Domain>();
                valueType = dom->valueType();

            }
        }
        QString typeName =  TypeHelper::type2HumanReadable(valueType);

        return typeName == sUNDEF ? "" : typeName;
    }catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return "";
}

QString IlwisObjectModel::rangeDefinition(bool defaultRange, bool calc, const QString& columnName) {
    try {
        IlwisTypes objectype = _ilwisobject->ilwisType();
        QString rangeString;
        if ( hasType( objectype, itCOVERAGE|itDOMAIN)){
            if ( objectype == itRASTER){
                IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
                if ( defaultRange){
                  rangeString = raster->datadef().domain()->range()->toString();
                }else{
                    if ( calc){
                        raster->loadData();
                        raster->statistics(NumericStatistics::pBASIC);
                    }
                  rangeString = raster->datadef().range()->toString();
                }
            } else if ( hasType( objectype , itFEATURE)){
                IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                ColumnDefinition coldef = features->attributeDefinitions().columndefinition(COVERAGEKEYCOLUMN);
                if ( coldef.isValid()){
                    if ( defaultRange)
                      rangeString =  coldef.datadef().domain()->range()->toString();
                    else{
                        if ( calc){
                            features->loadData();
                            std::vector<QVariant> data = features->attributeTable()->column(columnName);
                            if ( data.size() != 0){
                                std::vector<double> values(data.size());
                                int  i=0;
                                for(auto v : data)
                                    values[i++] = v.toDouble();
                                NumericStatistics stats; // realy like to do this with a template specialization of the proper calculate function, but the syntax was unclear to me
                                stats.calculate(values.begin(), values.end());
                                NumericRange *rng = new NumericRange(stats.prop(NumericStatistics::pMIN),stats.prop(NumericStatistics::pMAX));
                                features->attributeDefinitionsRef().columndefinitionRef(columnName).datadef().range(rng);
                            }
                        }
                        rangeString = coldef.datadef().range()->toString();
                    }
                }

            } else if ( hasType( objectype , itDOMAIN)){
                rangeString = _ilwisobject.as<Domain>()->range()->toString();
            }
        }
        return rangeString;
    }catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return "";

}

QString IlwisObjectModel::parentDomain() const {
    if ( hasType(_ilwisobject->ilwisType(),itDOMAIN)){
        IDomain domain = _ilwisobject.as<Domain>();
        if ( domain.isValid()){
            IDomain parentDomain = domain->parent();
            if ( parentDomain.isValid())
                return parentDomain->name();
        }
    }
    return "not defined";
}
QString IlwisObjectModel::pixSizeString() const{
    if (hasType(_ilwisobject->ilwisType(), itRASTER | itGEOREF)){
        double pixsizex=rUNDEF, pixsizey=rUNDEF;
        if ( hasType(_ilwisobject->ilwisType(), itRASTER)){
            Size<> sz = _ilwisobject.as<RasterCoverage>()->size();
            Envelope envelope = _ilwisobject.as<RasterCoverage>()->envelope();
            if ( envelope.isValid()){
                pixsizex = envelope.xlength() / sz.xsize();
                pixsizey = envelope.ylength() / sz.ysize();
                if ( _ilwisobject.as<RasterCoverage>()->coordinateSystem()->isLatLon()){
                    pixsizex *= 360.0;
                    pixsizey *= 360.0;
                }
            }
        }
        if ( hasType(_ilwisobject->ilwisType(), itGEOREF)){
            double pixsz = _ilwisobject.as<GeoReference>()->pixelSize();
            if ( pixsz == rUNDEF)
                return sUNDEF;

            if ( _ilwisobject.as<GeoReference>()->coordinateSystem()->isLatLon()){
                pixsz *= 360.0;
            }
            pixsizex = pixsizey = pixsz;
        }
        auto pixszstring = [&](double pixsize) ->QString{
            if ( pixsize > 50){
                return QString::number((int)pixsize);
            }else {
                return QString("%1").arg(pixsize,0,'f',1);
            }
        };
        return pixszstring(pixsizex) + " x " + pixszstring(pixsizey);

    }
    return "";
}

QString IlwisObjectModel::centerPixelLocation() const{
    Envelope envLatLon(Coordinate(-180,-61), Coordinate(180,81));
    QString code = QString("code=georef:type=corners,csy=epsg:4326,envelope=%1,gridsize=301 120,name=templocal").arg(envLatLon.toString());
    IGeoReference grf(code);
    if ( grf.isValid()){
        Envelope envelope;
        if ( hasType(_ilwisobject->ilwisType(), itCOVERAGE) )
            envelope = _ilwisobject.as<Coverage>()->envelope(true) ;
        else if ( hasType(_ilwisobject->ilwisType(), itCOORDSYSTEM))
            envelope = _ilwisobject.as<CoordinateSystem>()->envelope(true);

        if ( !envelope.isValid())
            return "";

        Coordinate crdCenter = envelope.center();
        Pixel pixCenter = grf->coord2Pixel(crdCenter);
        if ( pixCenter.isValid())
            return QString::number(pixCenter.x) + " " + QString::number(pixCenter.y);
    }
    return "";
}

QString IlwisObjectModel::valueType() const {
    if ( hasType(_ilwisobject->ilwisType(),itDOMAIN)){
        IDomain domain = _ilwisobject.as<Domain>();
        if ( domain.isValid()){
            IlwisTypes tp = domain->valueType();
            return TypeHelper::type2name(tp);
        }
    }
    return "";
}
QString IlwisObjectModel::getProperty(const QString &propertyname)
{
    try{
        QString property = ResourceModel::getProperty(propertyname);
        if ( property != sUNDEF)
            return property;
        if ( !_ilwisobject.isValid())
            return "";
        if ( propertyname == "latlonenvelope" || propertyname == "envelope"){
            if (hasType(_ilwisobject->ilwisType(), itCOVERAGE)){
                return _ilwisobject.as<Coverage>()->envelope(propertyname == "latlonenvelope").toString();
            } if ( hasType(_ilwisobject->ilwisType(), itCOORDSYSTEM)){
                return _ilwisobject.as<CoordinateSystem>()->envelope(propertyname == "latlonenvelope").toString();
            }
        }
        if ( propertyname == "pixelsize"){
            return pixSizeString();
        }
        if ( propertyname == "centerpixelboundingbox"){
            return centerPixelLocation();
        }
        if ( propertyname == "parentdomain"){
            return parentDomain();
        }
        if ( propertyname == "valuetype"){
            return valueType();
        }
        if ( propertyname == "recordcount"){
            if ( hasType(_ilwisobject->ilwisType(), itTABLE)){
                return QString::number(_ilwisobject.as<Table>()->recordCount());
            }
        }
        if ( propertyname == "columncount"){
            if ( hasType(_ilwisobject->ilwisType(), itTABLE)){
                return QString::number(_ilwisobject.as<Table>()->columnCount());
            }
        }
        if ( propertyname == "polygoncount"){
             if ( hasType(_ilwisobject->ilwisType(), itFEATURE)){
                IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                return QString::number(features->featureCount(itPOLYGON));
             }
        }
        if ( propertyname == "linecount"){
             if ( hasType(_ilwisobject->ilwisType(), itFEATURE)){
                IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                return QString::number(features->featureCount(itLINE));
             }
        }
        if ( propertyname == "pointcount"){
             if ( hasType(_ilwisobject->ilwisType(), itFEATURE)){
                IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                return QString::number(features->featureCount(itPOINT));
             }
        }
        if ( propertyname == "featurecount"){
             if ( hasType(_ilwisobject->ilwisType(), itFEATURE)){
                IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                return QString::number(features->featureCount());
             }
        }
        if ( propertyname == "majoraxis"){
             if ( hasType(_ilwisobject->ilwisType(), itELLIPSOID)){
                IEllipsoid ellipsoid = _ilwisobject.as<Ellipsoid>();
                return QString::number(ellipsoid->majorAxis());
             }
        }
        if ( propertyname == "minoraxis"){
             if ( hasType(_ilwisobject->ilwisType(), itELLIPSOID)){
                IEllipsoid ellipsoid = _ilwisobject.as<Ellipsoid>();
                return QString::number(ellipsoid->minorAxis());
             }
        }
        if ( propertyname == "flattening"){
             if ( hasType(_ilwisobject->ilwisType(), itELLIPSOID)){
                IEllipsoid ellipsoid = _ilwisobject.as<Ellipsoid>();
                return QString::number(ellipsoid->flattening());
             }
        }
        if ( propertyname == "excentricity"){
             if ( hasType(_ilwisobject->ilwisType(), itELLIPSOID)){
                IEllipsoid ellipsoid = _ilwisobject.as<Ellipsoid>();
                return QString::number(ellipsoid->excentricity());
             }
        }
        if (propertyname == "proj4def"){
            IlwisTypes objectype = _ilwisobject->ilwisType();
            if ( hasType( objectype, itPROJECTION | itCONVENTIONALCOORDSYSTEM)){
                IProjection proj;
                if ( hasType(objectype, itCONVENTIONALCOORDSYSTEM)){
                    IConventionalCoordinateSystem csyProj = _ilwisobject.as<ConventionalCoordinateSystem>();
                    if ( csyProj.isValid()){
                        proj = csyProj->projection();
                    }
                }else
                    proj = _ilwisobject.as<Projection>();
                if ( proj.isValid()){
                    return proj->toProj4();
                }
            }

        }

        return "";
    } catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return "";
}

bool IlwisObjectModel::canUse(const QString &id)
{
    if ( _ilwisobject.isValid()){
        bool ok;
        quint64 myid = id.toULongLong(&ok);
            if ( ok){
                IIlwisObject obj;
                obj.prepare(myid);
                if ( obj.isValid())
                    return _ilwisobject->canUse(obj.ptr());
            }
    }
    return false;
}

void IlwisObjectModel::setAttribute(const QString &attrname, const QString &value, const QString &extra)
{
    if ( _ilwisobject.isValid()){
        if ( attrname == "domain"){
            IDomain dom(value);
            if ( _ilwisobject->ilwisType() == itRASTER){
                IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
                if ( dom->id() != raster->datadefRef().domain()->id()){
                    raster->datadefRef().domain(dom);
                    raster->changed(true);
                    mastercatalog()->changeResource(raster->id(),"domain",dom->id(), true);
                }
            }
        }
    }
}

OperationModel *IlwisObjectModel::operation(const QString &id)
{
    bool ok;
    quint64 lid = id.toULongLong(&ok);
    if (!ok)
        return 0;
    return new OperationModel(lid, this);
}

CatalogModel *IlwisObjectModel::catalog(const QString &id)
{
    bool ok;
    quint64 lid = id.toULongLong(&ok);
    if (!ok)
        return 0;
    Resource res = mastercatalog()->id2Resource(lid);
    //res.addProperty("filter","container=" + res.url().toString());
    res.addProperty("locations", res.url().toString());
    return new CatalogModel(res, this);
}

bool IlwisObjectModel::isValid() const
{
    return _ilwisobject.isValid();
}

IIlwisObject IlwisObjectModel::object() const
{
    return _ilwisobject;
}

QString IlwisObjectModel::value2string(const QVariant &value, const QString &attrName)
{
    auto v2s = [](const ColumnDefinition& coldef, const QVariant& value)->QString {
            if ( coldef.isValid()){
                if ( coldef.datadef().domain()->ilwisType() == itTEXTDOMAIN)
                    return value.toString();
                return coldef.datadef().domain()->impliedValue(value).toString();
            }
            if ( value.toDouble() == rUNDEF)
                return sUNDEF;
            return value.toString();
    };
    if ( attrName != "") {
        IlwisTypes objectype = _ilwisobject->ilwisType();
        if ( hasType(objectype, itFEATURE)){
            IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
            ColumnDefinition coldef = features->attributeDefinitions().columndefinition(attrName);
            return v2s(coldef, value);

        }else if (hasType(objectype, itRASTER)){
             IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
             if ( raster->hasAttributes()){
                ColumnDefinition coldef = raster->attributeTable()->columndefinition(attrName);
                return v2s(coldef, value);
             }
             if ( raster->datadef().domain()->ilwisType() == itCOLORDOMAIN){
                 auto clr = ColorRangeBase::toColor(value, ColorRangeBase::cmRGBA);
                return ColorRangeBase::toString(clr,ColorRangeBase::cmRGBA)    ;
             }
        }
    }
    if ( value.toDouble() == rUNDEF)
        return sUNDEF;
    return value.toString();

}
