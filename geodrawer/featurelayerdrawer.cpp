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

    if ( hasType(prepType, DrawerInterface::ptGEOMETRY) && !isPrepared(DrawerInterface::ptGEOMETRY)){



        IFeatureCoverage features = coverage().as<FeatureCoverage>();
        if ( !features.isValid()){
            return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"FeatureCoverage", TR("Visualization"));
        }
        // set all to 0
        _indices = std::vector<VertexIndex>();
        _vertices = QVector<QVector3D>();
        _normals = QVector<QVector3D>();
        _colors = std::vector<VertexColor>();
        // get a description of how to render
        VisualAttribute attr = visualAttribute(activeAttribute());


        std::vector<std::shared_ptr<BaseSpatialAttributeSetter>> setters(5); // types are 1 2 4, for performance a vector is used thoug not all elements are used
        // for the moment I use the simple setters, in the future this will be representation dependent
        createAttributeSetter(features, setters, rootDrawer());

        _featureDrawings.resize(features->featureCount());
        int featureIndex = 0;
        for(const SPFeatureI& feature : features){
            QVariant value =  attr.columnIndex() != iUNDEF ? feature(attr.columnIndex()) : featureIndex;
            IlwisTypes geomtype = feature->geometryType();
             _featureDrawings[featureIndex] = setters[geomtype]->setSpatialAttributes(feature,_vertices,_normals);

            setters[geomtype]->setColorAttributes(attr,value,_featureDrawings[featureIndex],_colors) ;
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
        VisualAttribute attr = visualAttribute(activeAttribute());
        if ( !features.isValid()){
            return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"FeatureCoverage", TR("Visualization"));
        }
        _colors.resize(0);
        for(const SPFeatureI& feature : features){
            QVariant value =  attr.columnIndex() != iUNDEF ? feature(attr.columnIndex()) : featureIndex;
            IlwisTypes geomtype = feature->geometryType();
            setters[geomtype]->setColorAttributes(attr,value,_featureDrawings[featureIndex],_colors) ;
            ++featureIndex;
        }
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

void FeatureLayerDrawer::coverage(const ICoverage &cov)
{
    LayerDrawer::coverage(cov);
    setActiveVisualAttribute(sUNDEF);
    IFeatureCoverage features = coverage().as<FeatureCoverage>();

    for(int i = 0; i < features->attributeDefinitions().definitionCount(); ++i){
        IlwisTypes attrType = features->attributeDefinitions().columndefinition(i).datadef().domain()->ilwisType();
        if ( hasType(attrType, itNUMERICDOMAIN | itITEMDOMAIN | itTEXTDOMAIN)){
            VisualAttribute props(features->attributeDefinitions().columndefinition(i).datadef().domain(),i);
            if ( attrType == itNUMERICDOMAIN){
                SPNumericRange numrange = features->attributeDefinitions().columndefinition(i).datadef().range<NumericRange>();
                props.actualRange(NumericRange(numrange->min(), numrange->max(), numrange->resolution()));
            } else if ( attrType == itITEMDOMAIN){
                int count = features->attributeDefinitions().columndefinition(i).datadef().domain()->range<>()->count();
                props.actualRange(NumericRange(0, count - 1,1));
            }
            visualAttribute(features->attributeDefinitions().columndefinition(i).name(), props);
            // try to find a reasonable default for the activeattribute
            if ( activeAttribute() == sUNDEF){
                if ( features->attributeDefinitions().columnIndex(FEATUREVALUECOLUMN) != iUNDEF){
                    setActiveVisualAttribute(FEATUREVALUECOLUMN);
                }else if ( features->attributeDefinitions().columnIndex(COVERAGEKEYCOLUMN) != iUNDEF){
                    setActiveVisualAttribute(COVERAGEKEYCOLUMN);
                }
                else if ( hasType(features->attributeDefinitions().columndefinition(i).datadef().domain()->ilwisType(), itNUMERICDOMAIN)){
                    setActiveVisualAttribute(features->attributeDefinitions().columndefinition(i).name());
                }
            }
        }
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

    if(!_shaders.bind())
        return false;
    QMatrix4x4 mvp = rootDrawer()->mvpMatrix();

    _shaders.setUniformValue(_modelview, mvp);
    _shaders.setUniformValue(_vboAlpha, alpha());
    _shaders.enableAttributeArray(_vboNormal);
    _shaders.enableAttributeArray(_vboPosition);
    _shaders.enableAttributeArray(_vboColor);
    _shaders.setAttributeArray(_vboPosition, _vertices.constData());
    _shaders.setAttributeArray(_vboNormal, _normals.constData());
    _shaders.setAttributeArray(_vboColor, GL_FLOAT, (void *)_colors.data(),4);
    for(const auto& featureDrawing : _featureDrawings){
        if ( featureDrawing._geomtype == itPOINT){
            _shaders.setUniformValue(_scaleCenter, featureDrawing._center);
            _shaders.setUniformValue(_scaleFactor, (float)rootDrawer()->zoomScale());
        }else{
            _shaders.setUniformValue(_scaleFactor, 1.0f);
            if ( featureDrawing._geomtype == itLINE){
                glLineWidth(_lineWidth);
            }
        }

        for( const VertexIndex& featurePart : featureDrawing._indices)
            glDrawArrays(featurePart._oglType,featurePart._start,featurePart._count);
    }
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

    return QVariant();
}

void FeatureLayerDrawer::setAttribute(const QString &attrName, const QVariant &value)
{
    LayerDrawer::setAttribute(attrName, value);

    if ( attrName == "linewidth")
        _lineWidth = value.toFloat();
}



