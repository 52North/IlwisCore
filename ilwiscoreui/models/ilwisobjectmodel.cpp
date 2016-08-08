#include "coverage.h"
#include "ilwisobjectmodel.h"
#include "projection.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "proj4parameters.h"
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
            _ilwisobject.prepare(source);
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
            IConventionalCoordinateSystem csy;
            if (hasType(_ilwisobject->ilwisType(), itCOVERAGE)){
                csy = _ilwisobject.as<Coverage>()->coordinateSystem().as<ConventionalCoordinateSystem>();
            }else if (hasType(_ilwisobject->ilwisType(), itCOORDSYSTEM)){
                csy = _ilwisobject.as<ConventionalCoordinateSystem>();
            }else if ( hasType(_ilwisobject->ilwisType(), itGEOREF)){
                csy = _ilwisobject.as<GeoReference>()->coordinateSystem().as<ConventionalCoordinateSystem>();
            }
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

bool IlwisObjectModel::isAnonymous() const
{
    if (_ilwisobject.isValid()){
        return _ilwisobject->isAnonymous();
    }
    return false;
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

Resource& IlwisObjectModel::itemRef()
{
    if ( _ilwisobject.isValid())
        return _ilwisobject->resourceRef();
    return ResourceModel::itemRef();
}

const Resource &IlwisObjectModel::itemRef() const
{
    if ( _ilwisobject.isValid())
        return _ilwisobject->resourceRef();
    return ResourceModel::itemRef();
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
            if ( proj->isUsed(Projection::pvFALSEEASTING))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvFALSEEASTING, this));
            if ( proj->isUsed(Projection::pvFALSENORTHING))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvFALSENORTHING, this));
            if ( proj->isUsed(Projection::pvCENTRALMERIDIAN))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvCENTRALMERIDIAN, this));
            if ( proj->isUsed(Projection::pvCENTRALPARALLEL))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvCENTRALPARALLEL, this));
            if ( proj->isUsed(Projection::pvSTANDARDPARALLEL1))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvSTANDARDPARALLEL1, this));
            if ( proj->isUsed(Projection::pvSTANDARDPARALLEL2))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvSTANDARDPARALLEL2, this));
            if ( proj->isUsed(Projection::pvLATITUDEOFTRUESCALE))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvLATITUDEOFTRUESCALE, this));
            if ( proj->isUsed(Projection::pvSCALE))
                _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvSCALE, this));
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

QStringList IlwisObjectModel::quickProps() const
{
    QStringList result;
    try {

        if ( hasType(_ilwisobject->ilwisType(), itRASTER)){
            IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
            QString dom = QString("%1:%2").arg("domain").arg(raster->datadef().domain()->name());
            IlwisTypes tp = raster->datadef().domain()->ilwisType();
            QString domType =QString("%1:%2").arg("domain type").arg(TypeHelper::type2HumanReadable(tp));
            QString grf = QString("%1:%2").arg("georeference").arg(raster->georeference()->name());
            Size<> sz = raster->size();
            QString size = QString("%1:%2 %3 %4").arg("raster size").arg(sz.xsize()).arg(sz.ysize()).arg(sz.zsize());
            Envelope env = raster->envelope(true);
            QString e = QString("%1 %2 %3 %4").arg(env.min_corner().x,0,'f', 2).arg(env.min_corner().y,0,'f', 2).arg(env.max_corner().x,0,'f', 2).arg(env.max_corner().y,0,'f', 2);
            QString envelope = QString("envelope:" + e);


            result.append(grf);
            result.append(size);
            result.append(envelope);
            QString projInfo = projectionInfo();
            result.append("projection:" + projInfo);
            result.append(dom);
            result.append(domType);
            if ( raster->hasAttributes()){
                ITable tbl = raster->attributeTable();
                for(int i=0; i < tbl->columnCount(); ++i){
                    result.append(QString("attributes %1: %2").arg(i).arg(tbl->columndefinition(i).name()));
                }
            }
        }
        if ( hasType(_ilwisobject->ilwisType(), itFEATURE)){
            IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();

            QString pcount = QString("%1:%2").arg("polygon count").arg(getProperty("polygoncount"));
            QString lcount = QString("%1:%2").arg("line count").arg(getProperty("linecount"));
            QString pocount = QString("%1:%2").arg("point count").arg(getProperty("pointcount"));
            QString fcount = QString("%1:%2").arg("feature count").arg(getProperty("featurecount"));

            result.append(pcount);
            result.append(lcount);
            result.append(pocount);
            result.append(fcount);
            Envelope env = features->envelope(true);
            QString e = QString("%1 %2 %3 %4").arg(env.min_corner().x,0,'f', 2).arg(env.min_corner().y,0,'f', 2).arg(env.max_corner().x,0,'f', 2).arg(env.max_corner().y,0,'f', 2);
            QString envelope = QString("envelope:" + e);
            result.append(envelope);
            QString projInfo = projectionInfo();
            result.append("projection:" + projInfo);

            ITable tbl = features->attributeTable();
            for(int i=0; i < tbl->columnCount(); ++i){
                result.append(QString("attribute %1: %2").arg(i).arg(tbl->columndefinition(i).name()));
            }


        }
        if ( hasType(_ilwisobject->ilwisType(), itTABLE)){
            ITable tbl = _ilwisobject.as<Table>();
            result.append("record count:" + QString::number(tbl->recordCount()));
            result.append("column count:" + QString::number(tbl->columnCount()));
            for(int i=0; i < tbl->columnCount(); ++i){
                ColumnDefinition def = tbl->columndefinition(i);
                IlwisTypes tp = def.datadef().domain()->ilwisType();
                QString domType =QString("%1:%2").arg("domain type").arg(TypeHelper::type2HumanReadable(tp));
                result.append(QString("column %1: %2 %3").arg(i).arg(def.name()).arg(domType));
            }
        }
        if ( hasType(_ilwisobject->ilwisType(), itGEOREF)){
            IGeoReference gr = _ilwisobject.as<RasterCoverage>();
            Size<> sz = gr->size();
            QString size = QString("%1:%2 %3 %4").arg("raster size").arg(sz.xsize()).arg(sz.ysize()).arg(sz.zsize());
            Envelope env = gr->envelope();
            QString pxSize = QString("pixel size:%1").arg(gr->pixelSize());
            QString e = QString("%1 %2 %3 %4").arg(env.min_corner().x,0,'f', 2).arg(env.min_corner().y,0,'f', 2).arg(env.max_corner().x,0,'f', 2).arg(env.max_corner().y,0,'f', 2);
            QString envelope = QString("envelope:" + e);

            result.append(size);
            result.append(pxSize);
            result.append(envelope);
            QString projInfo = projectionInfo();
            result.append("projection:" + projInfo);
        }
        if ( hasType(_ilwisobject->ilwisType(), itDOMAIN)){
            IDomain dom = _ilwisobject.as<Domain>();
            IlwisTypes tp = dom->ilwisType();
            QString domType =QString("%1:%2").arg("domain type").arg(TypeHelper::type2HumanReadable(tp));
            QString vType = TypeHelper::type2HumanReadable(dom->valueType());
            QString valueType = QString("value type:" + vType);
            QString extra;
            if ( hasType(tp, itITEMDOMAIN)){
                extra = QString("item count:%1").arg(dom.as<ItemDomain<DomainItem>>()->count());
            }else if(hasType(tp,itNUMERICDOMAIN)){
                auto range = dom->range()->as<NumericRange>();
                extra = QString("numeric range:" + range->toString());
            }
            result.append(domType);
            result.append(valueType);
            if ( extra != "")
                result.append(extra);


        }
        return result;
    }
    catch(const ErrorObject& err){
        result.append("error:" + err.message());
    }


    return result;
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
    QString anonName = Identity::newAnonymousName();
    QString code = QString("code=georef:type=corners,csy=epsg:4326,envelope=%1,gridsize=301 120,name=%2").arg(envLatLon.toString()).arg(anonName);
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
QString IlwisObjectModel::getProperty(const QString &propertyname) const
{
    try{
        QString property = ResourceModel::getProperty(propertyname);
        if ( property != sUNDEF)
            return property;
        if ( !_ilwisobject.isValid())
            return "";
        if ( propertyname == "latlonenvelope" || propertyname == "envelope"){
            Envelope env;
            if (hasType(_ilwisobject->ilwisType(), itCOVERAGE)){
                env = _ilwisobject.as<Coverage>()->envelope(propertyname == "latlonenvelope");
            } if ( hasType(_ilwisobject->ilwisType(), itCOORDSYSTEM)){
                env =_ilwisobject.as<CoordinateSystem>()->envelope(propertyname == "latlonenvelope");
            }
            if ( env.isNull() || !env.isValid())
                return "unspecified";
            return env.toString();
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
        if ( propertyname == "georefid"){
            if ( hasType(_ilwisobject->ilwisType(), itRASTER)){
                IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
                if ( raster.isValid() && raster->georeference().isValid()){
                    return QString::number(raster->georeference()->id());
                }
            }
        }
        if ( propertyname == "georeferenceurl"){
            if ( hasType(_ilwisobject->ilwisType(), itRASTER)){
                IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
                if ( raster.isValid() && raster->georeference().isValid()){
                    return raster->georeference()->resource().url().toString();
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
                    raster->resourceRef().addProperty("domain", dom->id());
                    raster->changed(true);
                    mastercatalog()->changeResource(raster->id(),"domain",dom->id(), true);
                }
            }
        }else if ( attrname == "coordinatesystem"){
            QString def = value.trimmed();
            bool ok;
            value.toUInt(&ok);
            if ( ok){
                def = "code=epsg:" + value;
            }else if ( def.indexOf("+proj") >= 0){
                def = "code=proj4" + value;
            }else if ( def.toLower() == "wgs84" || def.toLower() == "wgs 84"){
                def = "code=epsg:4326";
            }
            ICoordinateSystem csy(def);
            if ( csy.isValid()){
                ICoverage coverage = _ilwisobject.as<Coverage>();
                if ( coverage.isValid()){
                    coverage->coordinateSystem(csy);

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
    return new CatalogModel(res, CatalogModel::ctDONTCARE, this);
}

void IlwisObjectModel::unload()
{
    if ( _ilwisobject.isValid()){
        _ilwisobject->unload();
    }
}

QString IlwisObjectModel::copy(const QString &newUrl, const QString &format, const QString &provider)
{
    try {
        if ( _ilwisobject.isValid()){
            quint64 id = mastercatalog()->name2id(newUrl, _ilwisobject->ilwisType());
            if ( id == i64UNDEF){
                _ilwisobject->connectTo(newUrl,format.toLower(), provider.toLower(), IlwisObject::cmOUTPUT);
                _ilwisobject->store();
                // original object must forget about its copy
                _ilwisobject->resetOutputConnector();
            }else{
                return "exists";
            }
        }
        return "ok";
    } catch(const ErrorObject& ){}

    return "error";
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

            QVariant impliedValue = coldef.datadef().domain()->impliedValue(value);
            if (coldef.datadef().domain()->valueType() == itNUMERICDOMAIN ) {
                double resolution = coldef.datadef().range().dynamicCast<NumericRange>()->resolution();
                if ( resolution == 0)
                    return QString::number(impliedValue.toDouble(), 'f', 3);
                if ( resolution == 1){
                    return QString::number(impliedValue.toLongLong());
                }
            }
            return impliedValue.toString();
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
            double resolution = raster->datadef().range().dynamicCast<NumericRange>()->resolution();
            if ( resolution == 0)
                return QString::number(value.toDouble(), 'f', 5);
            if ( resolution == 1){
                return QString::number(value.toLongLong());
            }
        }
    }
    if ( value.toDouble() == rUNDEF)
        return sUNDEF;
    //IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();

    return QString::number(value.toDouble(), 'f', 3);

}
