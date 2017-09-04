/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Solutions component.
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

#ifndef QTSERVICE_H
#define QTSERVICE_H
#include <map>
#include <QCoreApplication>

#if defined(Q_OS_WIN)
#  if !defined(QT_QTSERVICE_EXPORT) && !defined(QT_QTSERVICE_IMPORT)
#    define QT_QTSERVICE_EXPORT
#  elif defined(QT_QTSERVICE_IMPORT)
#    if defined(QT_QTSERVICE_EXPORT)
#      undef QT_QTSERVICE_EXPORT
#    endif
#    define QT_QTSERVICE_EXPORT __declspec(dllimport)
#  elif defined(QT_QTSERVICE_EXPORT)
#    undef QT_QTSERVICE_EXPORT
#    define QT_QTSERVICE_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTSERVICE_EXPORT
#endif

class QStringList;
class QtServiceControllerPrivate;

class QT_QTSERVICE_EXPORT QtServiceController
{
    Q_DECLARE_PRIVATE(QtServiceController)
public:
    enum StartupType
    {
	    AutoStartup = 0, ManualStartup
    };

    QtServiceController(const QString &name);
    virtual ~QtServiceController();

    bool isInstalled() const;
    bool isRunning() const;

    QString serviceName() const;
    QString serviceDescription() const;
    StartupType startupType() const;
    QString serviceFilePath() const;

    static bool install(const QString &serviceFilePath, const QString &account = QString(),
                const QString &password = QString());
    bool uninstall();

    bool start(const QStringList &arguments);
    bool start();
    bool stop();
    bool pause();
    bool resume();
    bool sendCommand(int code);

private:
    QtServiceControllerPrivate *d_ptr;
};

class QtServiceBasePrivate;

class QT_QTSERVICE_EXPORT QtService
{
    Q_DECLARE_PRIVATE(QtServiceBase)
public:

    enum MessageType
    {
	Success = 0, Error, Warning, Information
    };

    enum ServiceFlag
    {
        Default = 0x00,
        CanBeSuspended = 0x01,
        CannotBeStopped = 0x02,
        NeedsStopOnShutdown = 0x04
    };

    Q_DECLARE_FLAGS(ServiceFlags, ServiceFlag)

    QtService(const std::map<QString, QVariant>& args, const QString &name);
    virtual ~QtService();

    QString serviceName() const;

    QString serviceDescription() const;
    void setServiceDescription(const QString &description);

    QtServiceController::StartupType startupType() const;
    void setStartupType(QtServiceController::StartupType startupType);

    ServiceFlags serviceFlags() const;
    void setServiceFlags(ServiceFlags flags);

    int exec(const std::map<QString, QVariant>& args);

    void logMessage(const QString &message, MessageType type = Success,
                int id = 0, uint category = 0, const QByteArray &data = QByteArray());

    static QtService *instance();
    std::map<QString, QVariant> arguments() const;

protected:

    virtual void start();
    virtual void stop();
    virtual void pause();
    virtual void resume();
    virtual void processCommand(int code);

private:

    friend class QtServiceSysPrivate;
    QtServiceBasePrivate *d_ptr;
};



Q_DECLARE_OPERATORS_FOR_FLAGS(QtService::ServiceFlags)


#endif // QTSERVICE_H
