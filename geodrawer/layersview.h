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

#ifndef FBOINSGRENDERER_H
#define FBOINSGRENDERER_H

#include <QtQuick/QQuickFramebufferObject>
#include "kernel.h"
#include "drawers/layersviewcommandinterface.h"
#include <deque>

class LayersRenderer;

struct DrawerIdTag {
    DrawerIdTag(const Ilwis::Resource& resource) : _resource(resource){}
    DrawerIdTag(const QString& name, bool ascode) : _drawerName(name),_asCode(ascode){}
    Ilwis::Resource _resource;
    quint64 _drawerid = Ilwis::iUNDEF;
    QString _drawerName;
    QString _drawerCode;
    bool _asCode;
};

class LayersView : public QQuickFramebufferObject, public LayersViewCommandInterface
{
    Q_OBJECT

    Q_PROPERTY(QString viewerId READ viewerId CONSTANT)
public:
friend class LayersRenderer;

    LayersView();
    ~LayersView();


    Renderer *createRenderer() const;
    void addCommand(const QString& command, const QVariantMap& params);

    Q_INVOKABLE void addDataSource(const QString &url, const QString &typeName);
    Q_INVOKABLE void setAttribute(const QString &drawercode, const QVariantMap& value);
    Q_INVOKABLE void removeDrawer(const QString& namecode, bool ascode);
    Q_INVOKABLE void addDrawer(const QString &drawercode, const QVariantMap &properties);


private:
    QString viewerId() const;
    std::vector<DrawerIdTag> _datasources;
    std::map<QString, Ilwis::IOOptions> _specialdrawers;
    std::deque<DrawerIdTag> _removedDrawers;
    std::deque<std::pair<QString, QVariantMap>> _attributeQueue;
    quint64 _viewerId;

    static quint64 _baseViewerId;
};

#endif
