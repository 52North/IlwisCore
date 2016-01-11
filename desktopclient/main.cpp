#include <QtGui/QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtQml>

#include "startilwis.h"

int main(int argc, char *argv[])
{
        QGuiApplication app(argc, argv);


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
}
