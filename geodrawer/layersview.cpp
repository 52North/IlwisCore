/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include "visualizationmanager.h"
#include "coverage.h"
#include "layersview.h"
#include "layersrenderer.h"
#include "uicontextmodel.h"

#include <QtGui/QOpenGLFramebufferObject>

#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>

quint64 LayersView::_baseViewerId = 0;

LayersView::LayersView()
{
    _viewerId = _baseViewerId++;
    uicontext()->addViewer(this, _viewerId);
}

LayersView::~LayersView()
{
    uicontext()->removeViewer(_viewerId)    ;
}

QQuickFramebufferObject::Renderer *LayersView::createRenderer() const
{
    return new LayersRenderer();
}

void LayersView::addCommand(const QString &command, const QVariantMap &params)
{

}

void LayersView::addDataSource(const QString &url, const QString &typeName)
{
    try {
//        if ( url == "" || typeName == "")
//            return;


//        IlwisTypes tp = Ilwis::IlwisObject::name2Type(typeName);
//        if ( tp == itUNKNOWN)
//            return;


//        quint64 id = mastercatalog()->url2id(QUrl(url),tp);
//        if ( id == i64UNDEF){
//            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, TR("Visualization"), url);
//            return ;
//        }
//        Resource resource = mastercatalog()->id2Resource(id);
//        if (! resource.isValid()){
//            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, TR("Visualization"), url);
//            return ;
//        }
//        DrawerIdTag tag(resource);
//        _datasources.push_back(tag);

    } catch ( const ErrorObject& err){

    } catch ( const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
}

void LayersView::setAttribute(const QString &drawercode, const QVariantMap &values)
{
    _attributeQueue.push_back(std::pair<QString, QVariantMap>(drawercode, values));

}

void LayersView::removeDrawer(const QString &namecode, bool ascode)
{
//    _removedDrawers.push_back({namecode, ascode});
//    for(auto iter = _datasources.begin(); iter != _datasources.end(); ++iter){
//        if ( ascode){
//            if ( (*iter)._drawerCode == namecode){
//                _datasources.erase(iter);
//                break;
//            }
//        }else if ((*iter)._drawerName == namecode) {
//            _datasources.erase(iter);
//        }
//    }
}

void LayersView::addDrawer(const QString& drawercode, const QVariantMap& properties)
{
//    IOOptions opt;
//    for(QVariantMap::const_iterator iter = properties.begin(); iter != properties.end(); ++iter) {
//        opt << IOOptions::Option(iter.key(), iter.value());

//    }
//    _specialdrawers[drawercode] = opt;
}

void LayersView::addCommand(const QString &expression)
{
    OperationExpression expr(expression);
    if ( expr.isValid()){
        _commands.push_front(expr);
    }
}

QString LayersView::viewerId() const
{
    return QString::number(_viewerId);
}


