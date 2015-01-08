#include <QtGui/QGuiApplication>
#include <QtQml>
#include <QtQuick/QQuickView>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlQuery>
#include "kernel.h"
#include "errorobject.h"
#include "resource.h"
#include "ilwistype.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "catalogview.h"
#include "ilwiscontext.h"
#include "models/catalogfiltermodel.h"
#include "models/mastercatalogmodel.h"
#include "models/operationmodel.h"
#include "models/operationcatalogmodel.h"
#include "models/usermessagehandler.h"
#include "applicationformexpressionparser.h"
#include "models/tranquilizerhandler.h"
#include "models/visualizationmanager.h"
#include "models/objectvisualizationmodel.h"
#include "models/ilwisobjectmodel.h"
#include "models/attributemodel.h"
#include "models/domainitemmodel.h"
#include "models/operationsbykeymodel.h"
#include "models/uicontextmodel.h"
#include "models/visualizationmanager.h"

#define TEST_WORKINGDIR QString("file:///d:/dev/Ilwis/testdata")

using namespace Ilwis;
//using namespace Desktopclient;

int main(int argc, char *argv[])
{
    try{
        QGuiApplication app(argc, argv);

        QQmlApplicationEngine engine;
        QQmlContext *ctx = engine.rootContext();
        Ilwis::initIlwis();

        QFileInfo ilwisroot = context()->ilwisFolder();
        QString qmlpluginpath = ilwisroot.absoluteFilePath() + "/extensions/ui";

        engine.addImportPath(qmlpluginpath);
        engine.addPluginPath(qmlpluginpath);

        qmlRegisterType<MasterCatalogModel>("MasterCatalogModel",1,0,"MasterCatalogModel");
        qmlRegisterType<CatalogModel>("CatalogModel",1,0,"CatalogModel");
        qmlRegisterType<ResourceModel>("ResourceModel",1,0,"ResourceModel");
        qmlRegisterType<OperationCatalogModel>("OperationCatalogModel",1,0,"OperationCatalogModel");
        qmlRegisterType<OperationModel>("OperationModel",1,0,"OperationModel");
        qmlRegisterType<ApplicationFormExpressionParser>("ApplicationFormExpressionParser",1,0,"FormBuilder");
        qmlRegisterType<UserMessageHandler>("UserMessageHandler",1,0,"UserMessageHandler");
        qmlRegisterType<MessageModel>("MessageModel",1,0,"MessageModel");
        qmlRegisterType<TranquilizerHandler>("TranquilizerHandler",1,0, "TranquilizerHandler");
        qmlRegisterType<TranquilizerModel>("TranquilizerModel",1,0, "TranquilizerModel");
        qmlRegisterType<VisualizationManager>("VisualizationManager",1,0,"VisualizationManager");
        qmlRegisterType<ObjectVisualizationModel>("ObjectVisualizationModel",1,0,"ObjectVisualizationModel");
        qmlRegisterType<IlwisObjectModel>("IlwisObjectModel",1,0,"IlwisObjectModel");
        qmlRegisterType<AttributeModel>("AttributeModel",1,0,"AttributeModel");
        qmlRegisterType<DomainItemModel>("DomainItemModel",1,0,"DomainItemModel");
        qmlRegisterType<OperationsByKeyModel>("OperationsByKeyModel",1,0,"OperationsByKeyModel");
        qmlRegisterType<UIContextModel>("UIContextModel", 1,0, "UIContextModel");
        qmlRegisterType<PropertyEditorMetaData>("PropertyEditorMetaData", 1,0, "PropertyEditorMetaData");


        MasterCatalogModel mastercatalogmodel(ctx);

        ApplicationFormExpressionParser formbuilder;
        UserMessageHandler messageHandler;
        OperationCatalogModel operations;
        TranquilizerHandler tranquilizers;
        UIContextModel uiContext;
        uiContext.qmlContext(ctx);

        uiContext.addPropertyEditor(itLINE,"Style",PropertyEditorMetaData("Style", QUrl("http://someurl/bla.qml")));
        uiContext.addPropertyEditor(itLINE,"Representation",PropertyEditorMetaData("Representation", QUrl("http://someurl22/barbar.qml")));

        ctx->setContextProperty("mastercatalog", &mastercatalogmodel);
        ctx->setContextProperty("formbuilder", &formbuilder);
        ctx->setContextProperty("messagehandler", &messageHandler);
        ctx->setContextProperty("tranquilizerHandler", &tranquilizers);
        ctx->setContextProperty("operations", &operations);
        ctx->setContextProperty("uicontext", &uiContext);


        mastercatalogmodel.connect(&operations, &OperationCatalogModel::updateCatalog,&mastercatalogmodel, &MasterCatalogModel::updateCatalog );
        messageHandler.connect(kernel()->issues().data(), &IssueLogger::updateIssues,&messageHandler, &UserMessageHandler::addMessage );
        tranquilizers.connect(kernel(), &Kernel::updateTranquilizer, &tranquilizers, &TranquilizerHandler::updateTranquilizer,Qt::DirectConnection);
        tranquilizers.connect(kernel(), &Kernel::createTranquilizer, &tranquilizers, &TranquilizerHandler::createTranquilizer,Qt::DirectConnection);
        tranquilizers.connect(kernel(), &Kernel::removeTranquilizer, &tranquilizers, &TranquilizerHandler::removeTranquilizer);


        engine.load("qml/DesktopClient.qml");

        QObject *topLevel = engine.rootObjects().value(0);
        QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
        if ( !window ) {
            qWarning("Error: Your root item has to be a Window.");
            return -1;
        }
        mastercatalogmodel.root(window);
        window->show();
        return app.exec();
    }catch (const ErrorObject& err){
        qDebug() << err.message();
    }

    return 0;


}
