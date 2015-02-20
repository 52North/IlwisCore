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
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"



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
        _rootDrawer->pixelAreaSize(_viewPortSize);

        for(const Ilwis::OperationExpression& expr : gdrawer->_commands){
            quint64 id = commandhandler()->findOperationId(expr);
            if ( id != i64UNDEF) {
                QScopedPointer<OperationImplementation> oper(commandhandler()->create( expr));
                if ( !oper.isNull() && oper->isValid()) {
                    ExecutionContext ctx;
                    SymbolTable symTable;
                    QVariant v = qVariantFromValue((void *) _rootDrawer);
                    ctx._additionalInfo["rootdrawer"] = v;
                    oper->execute(&ctx, symTable);
                }
            }
        }
        gdrawer->_commands = std::deque<OperationExpression>();

        while( gdrawer->_attributeQueue.size() > 0){
            auto pair = gdrawer->_attributeQueue.front();
            gdrawer->_attributeQueue.pop_front();
            for(QVariantMap::const_iterator iter = pair.second.begin(); iter != pair.second.end(); ++iter) {
                _rootDrawer->drawerAttribute(pair.first,iter.key(), iter.value());
            }
        }
        _rootDrawer->prepare(Ilwis::Geodrawer::DrawerInterface::ptALL,Ilwis::IOOptions());

    } catch ( const ErrorObject& ){

    } catch ( const std::exception ex){

    }
}


