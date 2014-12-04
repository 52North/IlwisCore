#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "drawingcolor.h"
#include "drawerfactory.h"
#include "rootdrawer.h"
#include "featurelayerdrawer.h"
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

    if ( hasType(prepType, DrawerInterface::ptGEOMETRY)){
        _prepared |= ~DrawerInterface::ptGEOMETRY;
        std::vector<VertexPosition> vertices;
        _indices = std::vector<VertexIndex>();

        IFeatureCoverage features = coverage().as<FeatureCoverage>();
        if ( !features.isValid()){
            return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"FeatureCoverage", TR("Visualization"));
        }
        for(const SPFeatureI& feature : features){
            OpenGLHelper::getVertices(rootDrawer()->coordinateSystem(), features->coordinateSystem(), feature->geometry(), vertices, _indices);
        }


        if(!initGeometry(openglContext, vertices))
            return false;

        _prepared |= DrawerInterface::ptGEOMETRY;

    }

    return true;
}

bool FeatureLayerDrawer::draw(QOpenGLContext *openglContext, const IOOptions& options) {
    if ( !openglContext){
        return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR("Invalid OpenGL context passed"));
    }

    if ( !isActive())
        return false;

    if (!isPrepared(DrawerInterface::ptGEOMETRY)){
        if (!prepare(DrawerInterface::ptGEOMETRY, options, openglContext)){
            return false;
        }
    }
    _shaders.bind();

    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER,_vbo);

    int vertexLocation = _shaders.attributeLocation("aVertices");
    openglContext->functions()->glVertexAttribPointer(vertexLocation,3,GL_FLOAT,FALSE,0,0);
    openglContext->functions()->glEnableVertexAttribArray(vertexLocation);


    for(int i =0; i < _indices.size(); ++i){
        if ( _indices[i]._geomtype == itLINE){
            glDrawArrays(GL_LINE_STRIP,_indices[i]._start,_indices[i]._count);
        }
    }

    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    openglContext->functions()->glDisableVertexAttribArray(vertexLocation);

    _shaders.release();

    return true;
}
