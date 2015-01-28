#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "drawingcolor.h"
#include "drawerfactory.h"
#include "rootdrawer.h"
#include "table.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "colorlookup.h"
#include "representation.h"
#include "attributevisualproperties.h"
#include "featurelayerdrawer.h"
#include "tesselation/ilwistesselator.h"
#include "openglhelper.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_DRAWER(FeatureLayerDrawer)

FeatureLayerDrawer::FeatureLayerDrawer(DrawerInterface *parentDrawer, RootDrawer *rootdrawer) : LayerDrawer("FeatureLayerDrawer", parentDrawer, rootdrawer)
{
}

DrawerInterface *FeatureLayerDrawer::create(DrawerInterface *parentDrawer, RootDrawer *rootdrawer)
{
    return new FeatureLayerDrawer(parentDrawer, rootdrawer)    ;
}


bool FeatureLayerDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options, QOpenGLContext *openglContext)
{
    if(!LayerDrawer::prepare(prepType, options, openglContext))
        return false;

    if ( hasType(prepType, DrawerInterface::ptGEOMETRY) && !isPrepared(DrawerInterface::ptGEOMETRY)){
        std::vector<VertexPosition> vertices;
        std::vector<VertexColor> colors;

        _indices = std::vector<VertexIndex>();


        IFeatureCoverage features = coverage().as<FeatureCoverage>();
        if ( !features.isValid()){
            return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"FeatureCoverage", TR("Visualization"));
        }
        AttributeVisualProperties attr = visualAttribute(activeAttribute());
        //int columnIndex = features->attributeDefinitions().columnIndex(activeAttribute());
        for(const SPFeatureI& feature : features){
            quint32 noOfVertices = OpenGLHelper::getVertices(rootDrawer()->coordinateSystem(), features->coordinateSystem(), feature->geometry(), feature->featureid(), vertices, _indices, _boundaryIndex);
            for(int i =0; i < noOfVertices; ++i){
               QColor clr = attr.value2color(feature(activeAttribute()));
               colors.push_back(VertexColor(clr.redF(), clr.greenF(), clr.blueF(), 1.0));
            }

        }


        if(!initGeometry(openglContext, vertices, colors))
            return false;

        _prepared |= DrawerInterface::ptGEOMETRY;

    }

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


bool FeatureLayerDrawer::draw(QOpenGLContext *openglContext, const IOOptions&) {
    if ( !openglContext){
        return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR("Invalid OpenGL context passed"));
    }

    if ( !isActive())
        return false;

    if (!isPrepared()){
        return false;
    }

    _shaders.bind();

    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER,_vboPosition);

    int vertexLocation = _shaders.attributeLocation("position");
    openglContext->functions()->glVertexAttribPointer(vertexLocation,3,GL_FLOAT,FALSE,0,0);
    openglContext->functions()->glEnableVertexAttribArray(vertexLocation);

    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER,_vboColor);

    int colorLocation = _shaders.attributeLocation("vertexColor");
    openglContext->functions()->glVertexAttribPointer(colorLocation,4,GL_FLOAT,FALSE,0, 0);
    openglContext->functions()->glEnableVertexAttribArray(colorLocation);



    for(int i =0; i < _indices.size(); ++i){
        if ( _indices[i]._geomtype == itLINE){
            glDrawArrays(GL_LINE_STRIP,_indices[i]._start,_indices[i]._count);
        } else if ( _indices[i]._geomtype == itPOLYGON ){
            glDrawArrays(GL_TRIANGLE_FAN,_indices[i]._start,_indices[i]._count);
        }
    }

    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    openglContext->functions()->glDisableVertexAttribArray(colorLocation);
    openglContext->functions()->glDisableVertexAttribArray(vertexLocation);

    _shaders.release();

    return true;
}
