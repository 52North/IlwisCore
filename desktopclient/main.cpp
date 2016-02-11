#include <QtGui/QGuiApplication>
#include <QGLFormat>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtQml>

#include "startilwis.h"

int main(int argc, char *argv[])
{
        QGuiApplication app(argc, argv);

#ifdef _WIN32

        bool ver20 = false;
        if (QGLFormat::hasOpenGL()) {
            QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
            if (flags >= QGLFormat::OpenGL_Version_2_0)
                ver20 = true;
        }
        if (ver20) {

#endif

            QQmlApplicationEngine engine;
            qmlRegisterType<StartIlwis>("StartIlwis",1,0,"StartIlwis");
            StartIlwis start(0,&engine);
            engine.rootContext()->setContextProperty("startilwis", &start);
            engine.load("qml/StartWindow.qml");
            QObject *topLevel = engine.rootObjects().value(0);
            QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
            window->setDefaultAlphaBuffer(true);
            int ret =  app.exec();
            start.stop();
            return ret;

#ifdef _WIN32

        } else {

            MessageBox(0, (LPCTSTR)QString("OpenGL version 2.0 or better is required.\nAre the drivers of the graphics hardware properly installed?").utf16(), (LPCTSTR)QString("ILWIS4 Desktop cannot start.").utf16(), MB_OK | MB_ICONERROR);

            return 0;
        }

#endif

}
