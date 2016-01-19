#include "coverage.h"
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include "rootdrawer.h"
#include "table.h"
#include "layerdrawer.h"
#include "drawingcolor.h"
#include "drawers/drawerfactory.h"
#include "models/layermanager.h"
#include "layersrenderer.h"
#include "drawers/drawerinterface.h"
#include "layersview.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"



LayersRenderer::LayersRenderer(const QQuickFramebufferObject *fbo, bool active)
{
    if ( !_rootDrawer){
        _rootDrawer = new Ilwis::Geodrawer::RootDrawer(fbo, Ilwis::IOOptions());
        connect(_rootDrawer, &Ilwis::Geodrawer::BaseDrawer::updateRenderer,this, &LayersRenderer::updateRenderer );
    }
    _rootDrawer->active(active);
}

LayersRenderer::~LayersRenderer()
{
    delete _rootDrawer;
}


void LayersRenderer::render()
{
    try {
        if ( !_rootDrawer)
            return;

        if ( !_rootDrawer->isActive())
            return ;

        if ( _rootDrawer->is3D()){
            glEnable( GL_DEPTH_TEST);
            glDepthMask(true);
        }else
            glDisable( GL_DEPTH_TEST);

        QColor backgroundColor = _rootDrawer->attribute("backgroundcolor").value<QColor>();
        glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        _rootDrawer->draw( );

        glDisable(GL_BLEND);

        if ( _saveImagePath != "" && _fbo){
            QImage image = _fbo->toImage();
            QImage vflip = image.transformed(QMatrix().scale(1,-1));
            QUrl url(_saveImagePath);
            vflip.save(url.toLocalFile());
        }

        emit drawDone();
    }
    catch(const ErrorObject& ){}
    catch(const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
}

void LayersRenderer::cleanup()
{
    if ( _rootDrawer){
        _rootDrawer->cleanUp();
    }
}
void LayersRenderer::updateRenderer() {
    emit update();
}

void LayersRenderer::synchronize(QQuickFramebufferObject *item)
{
    try {

        bool needPrepare = false;
        LayersView *gdrawer = static_cast<LayersView *>(item);
        _viewPortSize =  QSize(gdrawer->width(), gdrawer->height());
        _windowSize = gdrawer->window()->size();
        _rootDrawer->pixelAreaSize(_viewPortSize);

        while(gdrawer->_attributerequests.size() > 0){
            auto pair = gdrawer->_attributerequests.front();
            gdrawer->_attributerequests.pop_front();
            QVariant var = _rootDrawer->attributeOfDrawer(pair.first, pair.second);
            gdrawer->_copiedAttributes[pair.first.toLower() + "|" + pair.second.toLower()] = var;
            needPrepare = true;
        }

        for(const Ilwis::OperationExpression& expr : gdrawer->_commands){
            quint64 id = commandhandler()->findOperationId(expr);
            if ( id != i64UNDEF) {
                QScopedPointer<OperationImplementation> oper(commandhandler()->create( expr));
                if ( !oper.isNull() && oper->isValid()) {
                    ExecutionContext ctx;
                    SymbolTable symTable;
                    QVariant v = qVariantFromValue((void *) _rootDrawer);
                    ctx._additionalInfo["rootdrawer"] = v;
                    if ( oper->execute(&ctx, symTable)){
                        Symbol sym = symTable.getSymbol("layerdrawer");
                        if ( sym.isValid()){
                            Ilwis::Geodrawer::DrawerInterface *drawer = static_cast<Ilwis::Geodrawer::DrawerInterface *>(sym._var.value<void *>());
                            if ( drawer){
                                ICoverage cov = drawer->attribute("coverage").value<ICoverage>();
                                if ( gdrawer->_manager)
                                    gdrawer->_manager->addDataSource(cov->source().url(),cov->ilwisType(),drawer);
                            }
                        }
                    }
                }
                needPrepare = true;
            }
        }
        gdrawer->_commands = std::deque<OperationExpression>();


        while( gdrawer->_attributeQueue.size() > 0){
            auto pair = gdrawer->_attributeQueue.front();
            gdrawer->_attributeQueue.pop_front();
            for(QVariantMap::const_iterator iter = pair.second.begin(); iter != pair.second.end(); ++iter) {
                if ( pair.first.toLower() == "view"){
                    handleRendererAttributes(iter.key(), iter.value())    ;
                }else {
                    _rootDrawer->drawerAttribute(pair.first.toLower(),iter.key(), iter.value());
                }
            }
            needPrepare = true;
        }
        if ( needPrepare){
            _rootDrawer->prepare(Ilwis::Geodrawer::DrawerInterface::ptALL,Ilwis::IOOptions());
           emit synchronizeDone();
        }


    } catch ( const ErrorObject& ){

    } catch ( const std::exception ex){

    }
}

void LayersRenderer::handleRendererAttributes(const QString& code, const QVariant& value){
    if ( code == "saveimage")    {
        //QString path = value.toString();
        _saveImagePath = value.toString();
    }
}

