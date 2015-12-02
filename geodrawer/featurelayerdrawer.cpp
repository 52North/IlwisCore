#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "drawingcolor.h"
#include "drawers/drawerfactory.h"
#include "rootdrawer.h"
#include "table.h"
#include "range.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "colorrange.h"
#include "colorlookup.h"
#include "itemdomain.h"
#include "representation.h"
#include "drawers/attributevisualproperties.h"
#include "drawers/drawerattributesetter.h"
#include "drawers/drawerattributesetterfactory.h"
#include "drawerattributesetters/basespatialattributesetter.h"
#include "featurelayerdrawer.h"
#include "tesselation/ilwistesselator.h"
//#include "openglhelper.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_DRAWER(FeatureLayerDrawer)

FeatureLayerDrawer::FeatureLayerDrawer(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) : LayerDrawer("FeatureLayerDrawer", parentDrawer, rootdrawer, options)
{
    _vertexShader = "featurevertexshader_nvdia.glsl";
    _fragmentShader = "featurefragmentshader_nvdia.glsl";
}

DrawerInterface *FeatureLayerDrawer::create(DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options)
{
    return new FeatureLayerDrawer(parentDrawer, rootdrawer, options)    ;
}


void createAttributeSetter(const IFeatureCoverage& features, std::vector<std::shared_ptr<BaseSpatialAttributeSetter>>& setters, RootDrawer *rootdrawer)
{
    QVariant v = qVariantFromValue((void *) rootdrawer);
    IOOptions opt("rootdrawer", v);
    setters[itPOINT].reset( DrawerAttributeSetterFactory::create<BaseSpatialAttributeSetter>("simplepointsetter",opt));
    setters[itLINE].reset( DrawerAttributeSetterFactory::create<BaseSpatialAttributeSetter>("simplelinesetter",  opt));
    setters[itPOLYGON].reset( DrawerAttributeSetterFactory::create<BaseSpatialAttributeSetter>("simplepolygonsetter", opt));
    for(int i=0; i < setters.size(); ++i){
        if(setters[i]){
            setters[i]->sourceCsySystem(features->coordinateSystem());
        }
    }
}

bool FeatureLayerDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options)
{
    if(!LayerDrawer::prepare(prepType, options))
        return false;

    if ( hasType(prepType, ptSHADERS) && !isPrepared(ptSHADERS)){
        _vboColor = _shaders.attributeLocation("vertexColor");
        _scaleCenter = _shaders.uniformLocation("scalecenter");
        _scaleFactor = _shaders.uniformLocation("scalefactor");

        _prepared |= DrawerInterface::ptSHADERS;
    }
    if ( hasType(prepType, DrawerInterface::ptGEOMETRY) && !isPrepared(DrawerInterface::ptGEOMETRY)){



        IFeatureCoverage features = coverage().as<FeatureCoverage>();
        if ( !features.isValid()){
            return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"FeatureCoverage", TR("Visualization"));
        }
        // set all to 0
        //_indices = std::vector<VertexIndex>();
        _vertices = QVector<QVector3D>();
        _normals = QVector<QVector3D>();
        _colors = std::vector<VertexColor>();
        // get a description of how to render
        VisualAttribute attr = visualProperty(activeAttribute());


        std::vector<std::shared_ptr<BaseSpatialAttributeSetter>> setters(5); // types are 1 2 4, for performance a vector is used thoug not all elements are used
        // for the moment I use the simple setters, in the future this will be representation dependent
        createAttributeSetter(features, setters, rootDrawer());

        _featureDrawings.resize(features->featureCount());
        int featureIndex = 0;
        for(const SPFeatureI& feature : features){
            QVariant value =  attr.columnIndex() != iUNDEF ? feature(attr.columnIndex()) : featureIndex;
            IlwisTypes geomtype = feature->geometryType();
             _featureDrawings[featureIndex] = setters[geomtype]->setSpatialAttributes(feature,_vertices,_normals);
            const QColor& clr = geomtype == itPOLYGON ? _boundaryColor : _lineColor;
            setters[geomtype]->setColorAttributes(attr,value,clr,_featureDrawings[featureIndex],_colors) ;
            ++featureIndex;
        }
        // implicity the redoing of the geometry is also redoing the representation stuff(a.o. colors)
        _prepared |= ( DrawerInterface::ptGEOMETRY | DrawerInterface::ptRENDER);

    }
    if ( hasType(prepType, DrawerInterface::ptRENDER) && !isPrepared(DrawerInterface::ptRENDER)){
        IFeatureCoverage features = coverage().as<FeatureCoverage>();
        int featureIndex = 0;
        std::vector<std::shared_ptr<BaseSpatialAttributeSetter>> setters(5); // types are 1 2 4, for performance a vector is used thoug not all elements are used
        // for the moment I use the simple setters, in the future this will be representation dependent
        createAttributeSetter(features, setters, rootDrawer());
        VisualAttribute attr = visualProperty(activeAttribute());
        if ( !features.isValid()){
            return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"FeatureCoverage", TR("Visualization"));
        }
        _colors.resize(0);
        bool polygononly = false;
        if ( options.contains("polygononly"))
            polygononly = options["polygononly"].toBool();

        for(const SPFeatureI& feature : features){
            if ( polygononly && feature->geometryType() != itPOLYGON){
                ++featureIndex;
                continue;
            }
            QVariant value =  attr.columnIndex() != iUNDEF ? feature(attr.columnIndex()) : featureIndex;
            IlwisTypes geomtype = feature->geometryType();
            const QColor& clr = geomtype == itPOLYGON ? _boundaryColor : _lineColor;
            setters[geomtype]->setColorAttributes(attr,value,clr,_featureDrawings[featureIndex],_colors) ;
            ++featureIndex;
        }
        _prepared |= DrawerInterface::ptRENDER;
    }

    //initialize();
    return true;
}

void FeatureLayerDrawer::unprepare(DrawerInterface::PreparationType prepType)
{
    LayerDrawer::unprepare(prepType);

    if ( hasType(prepType, DrawerInterface::ptGEOMETRY))    {
        _prepared &= ~ ptGEOMETRY;
    }
}

void FeatureLayerDrawer::setActiveVisualAttribute(const QString &attr)
{
    IFeatureCoverage features = coverage().as<FeatureCoverage>();
    if ( features.isValid())    {
        if ( features->attributeDefinitions().columnIndex(attr) != iUNDEF){

            IRepresentation newrpr = Representation::defaultRepresentation(features->attributeDefinitions().columndefinition(attr).datadef().domain());
            if ( newrpr.isValid()){
                LayerDrawer::setActiveVisualAttribute(attr);
            }
        }
    }
}

void FeatureLayerDrawer::resetVisualProperty(const QString& propertyName, const IRepresentation& rpr){
   IFeatureCoverage features = coverage().as<FeatureCoverage>();
    for(int i = 0; i < features->attributeDefinitions().definitionCount(); ++i){
        const ColumnDefinition& coldef = features->attributeDefinitions().columndefinitionRef(i);
        if ( coldef.name() == propertyName){
            IlwisTypes attrType = coldef.datadef().domain()->ilwisType();
            if ( hasType(attrType, itNUMERICDOMAIN | itITEMDOMAIN | itTEXTDOMAIN)){
                VisualAttribute props = createVisualProperty(coldef, i, rpr);
                visualProperty(coldef.name(), props);
                break;
            }
        }
    }
}

void FeatureLayerDrawer::coverage(const ICoverage &cov)
{
    LayerDrawer::coverage(cov);
    setActiveVisualAttribute(sUNDEF);
    IFeatureCoverage features = coverage().as<FeatureCoverage>();

    for(int i = 0; i < features->attributeDefinitions().definitionCount(); ++i){
        const ColumnDefinition& coldef = features->attributeDefinitions().columndefinitionRef(i);
        IlwisTypes attrType = coldef.datadef().domain()->ilwisType();
        if ( hasType(attrType, itNUMERICDOMAIN | itITEMDOMAIN | itTEXTDOMAIN)){
            VisualAttribute props = createVisualProperty(coldef, i);
            visualProperty(coldef.name(), props);
        }
    }
    // try to find a reasonable default for the activeattribute
    for(int i = 0; i < features->attributeDefinitions().definitionCount(); ++i){
        if ( activeAttribute() == sUNDEF){
            const ColumnDefinition& coldef = features->attributeDefinitions().columndefinitionRef(i);
            if ( features->attributeDefinitions().columnIndex(FEATUREVALUECOLUMN) != iUNDEF){
                setActiveVisualAttribute(FEATUREVALUECOLUMN);
            }else if ( features->attributeDefinitions().columnIndex(COVERAGEKEYCOLUMN) != iUNDEF){
                setActiveVisualAttribute(COVERAGEKEYCOLUMN);
            }
            else if ( hasType(coldef.datadef().domain()->ilwisType(), itNUMERICDOMAIN)){
                setActiveVisualAttribute(coldef.name());
            }
        }else
            break;
    }
}

ICoverage FeatureLayerDrawer::coverage() const
{
    return SpatialDataDrawer::coverage();
}

DrawerInterface::DrawerType FeatureLayerDrawer::drawerType() const
{
    return DrawerInterface::dtMAIN;
}


bool FeatureLayerDrawer::draw(const IOOptions& )
{
    if ( !isActive())
        return false;

    if (!isPrepared()){
        return false;
    }

    //qDebug() << id();
    if(!_shaders.bind())
        return false;

    if (_colors.size() == 0 || _vertices.size() == 0)
        return false;

    QMatrix4x4 mvp = rootDrawer()->mvpMatrix();
    float oldwidth;
    glGetFloatv(GL_LINE_WIDTH,&oldwidth);

    _shaders.setUniformValue(_modelview, mvp);
    _shaders.setUniformValue(_vboAlpha, alpha());
    _shaders.enableAttributeArray(_vboNormal);
    _shaders.enableAttributeArray(_vboPosition);
    _shaders.enableAttributeArray(_vboColor);
    //TODO: does setAttributeArray  have to be done each draw?
    _shaders.setAttributeArray(_vboPosition, _vertices.constData());
    _shaders.setAttributeArray(_vboNormal, _normals.constData());
    _shaders.setAttributeArray(_vboColor, GL_FLOAT, (void *)_colors.data(),4);
    for(const FeatureDrawing& featureDrawing : _featureDrawings){
        if ( featureDrawing._geomtype == itPOINT){
            _shaders.setUniformValue(_scaleCenter, featureDrawing._center);
            _shaders.setUniformValue(_scaleFactor, (float)rootDrawer()->zoomScale());
        }else{
            _shaders.setUniformValue(_scaleFactor, 1.0f);
            if ( featureDrawing._geomtype == itLINE){
                glLineWidth(_lineWidth);
            }else if (featureDrawing._geomtype == itPOLYGON){
                glLineWidth(_boundarywidth);
            }
        }

        for( const VertexIndex& featurePart : featureDrawing._indices){
            bool ok = true;
            if (featureDrawing._geomtype == itPOLYGON ){
                if (featurePart._oglType == GL_LINE_STRIP)
                    ok = _showBoundaries;
                else if (featurePart._oglType == GL_TRIANGLE_FAN){
                    ok = _showAreas;
                }
            }
            if ( ok)
                glDrawArrays(featurePart._oglType,featurePart._start,featurePart._count);
        }
    }
    glLineWidth(oldwidth);
    _shaders.disableAttributeArray(_vboNormal);
    _shaders.disableAttributeArray(_vboPosition);
    _shaders.disableAttributeArray(_vboColor);
    _shaders.release();

    return true;
}

QVariant FeatureLayerDrawer::attribute(const QString &attrName) const
{
    QVariant var = LayerDrawer::attribute(attrName);
    if ( var.isValid())
        return var;

    if ( attrName == "linewidth")
        return _lineWidth;
    if ( attrName == "polygonboundaries")
        return _showBoundaries;
    if ( attrName == "polygonareas"){
        return _showAreas;
    }
    if ( attrName == "areatransparency")
        return _areaTransparency;
    if ( attrName == "boundarycolor")
        return _boundaryColor;
    if ( attrName == "linecolor")
        return _lineColor;
    if ( attrName == "boundarywidth")
        return _boundarywidth;

    return QVariant();
}

void FeatureLayerDrawer::setAttribute(const QString &attrName, const QVariant &value)
{
    LayerDrawer::setAttribute(attrName, value);

    if ( attrName == "linewidth")
        _lineWidth = value.toFloat();
    if ( attrName == "boundarywidth")
        _boundarywidth = value.toFloat();
    if ( attrName == "polygonboundaries")
        _showBoundaries = value.toBool();
    if ( attrName == "polygonareas")
        _showAreas = value.toBool();
    if ( attrName == "areatransparency")
        _areaTransparency = value.toFloat();
    if ( attrName == "boundarycolor")
        _boundaryColor = value.value<QColor>();
    if ( attrName == "linecolor")
        _lineColor = value.value<QColor>();
}



