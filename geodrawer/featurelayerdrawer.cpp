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
#include "openglhelper.h"

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
        AttributeVisualProperties attr = visualAttribute(activeAttribute());


        std::vector<std::shared_ptr<BaseSpatialAttributeSetter>> setters(5); // types are 1 2 4, for performance a vector is used thoug not all elements are used
        // for the moment I use the simple setters, in the future this will be representation dependent
        QVariant v = qVariantFromValue((void *) rootDrawer());
        IOOptions opt("rootdrawer", v);
        setters[itPOINT].reset( DrawerAttributeSetterFactory::create<BaseSpatialAttributeSetter>("simplepointsetter",opt));
        setters[itLINE].reset( DrawerAttributeSetterFactory::create<BaseSpatialAttributeSetter>("simplelinesetter",  opt));
        setters[itPOLYGON].reset( DrawerAttributeSetterFactory::create<BaseSpatialAttributeSetter>("simplepolygonsetter", opt));
        for(int i=0; i < setters.size(); ++i){
            if(setters[i]){
                setters[i]->sourceCsySystem(features->coordinateSystem());
            }
        }

        _featureDrawings.resize(features->featureCount());
        int featureIndex = 0;
        for(const SPFeatureI& feature : features){
            QVariant value =  attr.columnIndex() != iUNDEF ? feature(attr.columnIndex()) : featureIndex;
            IlwisTypes geomtype = feature->geometryType();
            _featureDrawings[featureIndex] = setters[geomtype]->setSpatialAttributes(feature,_vertices,_normals);
            for(int i =0; i < _featureDrawings[featureIndex].size(); ++i)
                setters[geomtype]->setColorAttributes(attr,value,_featureDrawings[featureIndex][i]._start,_featureDrawings[featureIndex][i]._count,_colors) ;
            ++featureIndex;
        }
        _prepared |= DrawerInterface::ptGEOMETRY;

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
        if ( hasType(attrType, itNUMERICDOMAIN | itITEMDOMAIN)){
            AttributeVisualProperties props(features->attributeDefinitions().columndefinition(i).datadef().domain());
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

    _shaders.setUniformValue(_modelview, rootDrawer()->mvpMatrix());
    _shaders.enableAttributeArray(_vboNormal);
    _shaders.enableAttributeArray(_vboPosition);
    _shaders.enableAttributeArray(_vboColor);
    _shaders.setAttributeArray(_vboPosition, _vertices.constData());
    _shaders.setAttributeArray(_vboNormal, _normals.constData());
    _shaders.setAttributeArray(_vboColor, GL_FLOAT, (void *)_colors.data(),4);
    for(const auto& featureDrawing : _featureDrawings){
        for( const VertexIndex& featurePart : featureDrawing)
            glDrawArrays(featurePart._oglType,featurePart._start,featurePart._count);
     }
    _shaders.disableAttributeArray(_vboNormal);
    _shaders.disableAttributeArray(_vboPosition);
    _shaders.disableAttributeArray(_vboColor);
    _shaders.release();

    return true;
}


