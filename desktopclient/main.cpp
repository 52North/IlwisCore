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
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "models/catalogfiltermodel.h"
#include "models/mastercatalogmodel.h"
#include "models/operationmodel.h"
#include "models/operationcatalogmodel.h"
#include "models/usermessagehandler.h"
#include "applicationformexpressionparser.h"
#include "models/tranquilizerhandler.h"
#include "models/visualizationmanager.h"
#include "models/coveragelayermodel.h"
#include "models/ilwisobjectmodel.h"
#include "models/attributemodel.h"
#include "models/domainitemmodel.h"
#include "models/operationsbykeymodel.h"
#include "models/uicontextmodel.h"
#include "models/visualizationmanager.h"
#include "models/projectionparametermodel.h"
#include "models/workflowmodel.h"
#include "models/visualattributemodel.h"
#include "models/tablemodel.h"
#include "ilwiscoreui/propertyeditors/representationsetter.h"
#include "keyfilter.h"

#define TEST_WORKINGDIR QString("file:///s:/data/coding/ilwis/2014-03-18_testdata")

using namespace Ilwis;
//using namespace Desktopclient;



int main(int argc, char *argv[])
{
    try{
        QGuiApplication app(argc, argv);

        QQmlApplicationEngine engine;
        QQmlContext *ctx = engine.rootContext();
        Ilwis::initIlwis(Ilwis::rmDESKTOP);

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
        qmlRegisterType<LayerManager>("LayerManager",1,0,"LayerManager");
        qmlRegisterType<CoverageLayerModel>("CoverageLayerModel",1,0,"CoverageLayerModel");
        qmlRegisterType<IlwisObjectModel>("IlwisObjectModel",1,0,"IlwisObjectModel");
        qmlRegisterType<AttributeModel>("AttributeModel",1,0,"AttributeModel");
        qmlRegisterType<DomainItemModel>("DomainItemModel",1,0,"DomainItemModel");
        qmlRegisterType<OperationsByKeyModel>("OperationsByKeyModel",1,0,"OperationsByKeyModel");
        qmlRegisterType<UIContextModel>("UIContextModel", 1,0, "UIContextModel");
        qmlRegisterType<VisualAttributeEditor>("VisualAttributeEditor", 1,0, "VisualAttributeEditor");
        qmlRegisterType<RepresentationSetter>("RepresentationSetter", 1,0, "RepresentationSetter");
        qmlRegisterType<RepresentationElement>("RepresentationElement", 1,0, "RepresentationElement");
        qmlRegisterType<ProjectionParameterModel>("ProjectionParameterModel", 1,0, "ProjectionParameterModel");
        qmlRegisterType<WorkflowModel>("WorkflowModel", 1,0, "WorkflowModel");
        qmlRegisterType<VisualAttributeModel>("VisualAttributeModel", 1,0,"VisualAttributeModel");
        qmlRegisterType<TableModel>("TableModel", 1,0,"TableModel");


        MasterCatalogModel mastercatalogmodel(ctx);

        ApplicationFormExpressionParser formbuilder;
        UserMessageHandler messageHandler;
        OperationCatalogModel operations;
        TranquilizerHandler tranquilizers;
        uicontext()->qmlContext(ctx);

        //uiContext.addPropertyEditor(itLINE,"Style",PropertyEditorMetaData("Style", QUrl("http://someurl/bla.qml")));
        //uicontext()->addPropertyEditor(itLINE,TR("Representation"),PropertyEditor(TR("Representation"), QUrl("RepresentationProperties.qml")));

        ctx->setContextProperty("mastercatalog", &mastercatalogmodel);
        ctx->setContextProperty("formbuilder", &formbuilder);
        ctx->setContextProperty("messagehandler", &messageHandler);
        ctx->setContextProperty("tranquilizerHandler", &tranquilizers);
        ctx->setContextProperty("operations", &operations);
        ctx->setContextProperty("uicontext", uicontext().get());


        mastercatalogmodel.connect(&operations, &OperationCatalogModel::updateCatalog,&mastercatalogmodel, &MasterCatalogModel::updateCatalog );
        messageHandler.connect(kernel()->issues().data(), &IssueLogger::updateIssues,&messageHandler, &UserMessageHandler::addMessage );
        tranquilizers.connect(kernel(), &Kernel::updateTranquilizer, &tranquilizers, &TranquilizerHandler::updateTranquilizer,Qt::DirectConnection);
        tranquilizers.connect(kernel(), &Kernel::createTranquilizer, &tranquilizers, &TranquilizerHandler::createTranquilizer,Qt::DirectConnection);
        tranquilizers.connect(kernel(), &Kernel::removeTranquilizer, &tranquilizers, &TranquilizerHandler::removeTranquilizer);


        engine.load("qml/DesktopClient.qml");

        QObject *topLevel = engine.rootObjects().value(0);
        QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
        window->setIcon(QIcon("./qml/images/ilwis4.bmp"));
        if ( !window ) {
            qWarning("Error: Your root item has to be a Window.");
            return -1;
        }
       // mastercatalogmodel.root(window);
        window->show();
        KeyFilter keys;
        app.installEventFilter(&keys);
        int ret =  app.exec();
        Ilwis::exitIlwis();

        return ret;
    }catch (const ErrorObject& err){
        qDebug() << err.message();
    }

    return 0;


}
