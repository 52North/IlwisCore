#include "coverage.h"
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include "rootdrawer.h"
#include "table.h"
#include "layerdrawer.h"
#include "drawingcolor.h"
#include "drawerfactory.h"
#include "models/visualizationmanager.h"
#include "layersrenderer.h"
#include "drawers/drawerinterface.h"
#include "layersview.h"


LayersRenderer::LayersRenderer()
{
}

LayersRenderer::~LayersRenderer()
{
}


void LayersRenderer::render()
{
    if ( !_rootDrawer)
        return;

    if ( !_rootDrawer->isActive())
        return ;

    glDepthMask(true);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _rootDrawer->draw();
}

void LayersRenderer::cleanup()
{
    if ( _rootDrawer){
        _rootDrawer->cleanUp();
    }
}

void LayersRenderer::synchronize(QQuickFramebufferObject *item)
{
    try {
        if ( !_rootDrawer){
            _rootDrawer = new Ilwis::Geodrawer::RootDrawer(Ilwis::IOOptions());
        }
        LayersView *gdrawer = static_cast<LayersView *>(item);
        _viewPortSize =  QSize(gdrawer->width(), gdrawer->height());
        _windowSize = gdrawer->window()->size();
        _originInLocalCS = QPointF(gdrawer->x(), gdrawer->y());
        _originInWindowCS = gdrawer->window()->contentItem()->mapFromItem(gdrawer, _originInLocalCS);

        //int sz = gdrawer->_removedDrawers.size();
        while(gdrawer->_removedDrawers.size() > 0){
            DrawerIdTag drawerid = gdrawer->_removedDrawers.front();
            gdrawer->_removedDrawers.pop_front();
            _rootDrawer->removeDrawer(drawerid._drawerName, drawerid._asCode);
        }


        for(auto& tag: gdrawer->_datasources){
            int drcount = _rootDrawer->drawerCount(Geodrawer::ComplexDrawer::dtMAIN);
            bool alreadyPresent = false;
            for(int  i = 0; i < drcount; ++i){
                auto& drawer = _rootDrawer->drawer(i);
                if ( drawer){
                    if ( tag._drawerid == drawer->id()){
                        alreadyPresent = true;
                        break;
                    }
                }
            }
            if ( !alreadyPresent){
                ICoverage coverage(tag._resource);
                if ( !coverage.isValid()){
                    ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, TR("Visualization"), tag._resource.url().toString());
                    return;
                }
                coverage->loadData(); // loaddata now so that all attribute info( ranges) is known; we need to load the data anyway.
                Geodrawer::LayerDrawer *layerdrawer = Geodrawer::DrawerFactory::create<Geodrawer::LayerDrawer>(coverage->ilwisType(), _rootDrawer, _rootDrawer, IOOptions());
                if ( !layerdrawer)
                    return;

                layerdrawer->coverage(coverage);
                _rootDrawer->pixelAreaSize(_viewPortSize);
                if ( _rootDrawer->drawerCount(Geodrawer::ComplexDrawer::dtMAIN) == 0)
                    _rootDrawer->coordinateSystem(coverage->coordinateSystem());
                _rootDrawer->newDrawer(layerdrawer,false);
                tag._drawerid = layerdrawer->id();
                tag._drawerCode = layerdrawer->code();
                tag._drawerName = layerdrawer->name();
            }
        }
        for(auto& spdrawer: gdrawer->_specialdrawers){
            Geodrawer::DrawerInterface *drawer = Geodrawer::DrawerFactory::create<>(spdrawer.first, _rootDrawer, _rootDrawer, spdrawer.second);
            if ( drawer){
                _rootDrawer->addDrawer(drawer,drawer->drawerType(),drawer->defaultOrder());
            }
        }
        gdrawer->_specialdrawers = std::map<QString, Ilwis::IOOptions>();



        while( gdrawer->_attributeQueue.size() > 0){
            auto pair = gdrawer->_attributeQueue.front();
            gdrawer->_attributeQueue.pop_front();
            if ( pair.first != "command"){
                for(QVariantMap::const_iterator iter = pair.second.begin(); iter != pair.second.end(); ++iter) {
                    _rootDrawer->drawerAttribute(pair.first,iter.key(), iter.value());
                }
            }
        }
        _rootDrawer->pixelAreaSize(_viewPortSize);
        _rootDrawer->prepare(Ilwis::Geodrawer::DrawerInterface::ptALL,Ilwis::IOOptions());
    } catch ( const ErrorObject& ){

    } catch ( const std::exception ex){

    }
}


