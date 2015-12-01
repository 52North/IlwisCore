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
#include "workflowmetadataformbuilder.h"
#include "models/tranquilizerhandler.h"
#include "models/layermanager.h"
#include "models/coveragelayermodel.h"
#include "models/ilwisobjectmodel.h"
#include "models/attributemodel.h"
#include "models/domainitemmodel.h"
#include "models/operationsbykeymodel.h"
#include "models/uicontextmodel.h"
#include "models/projectionparametermodel.h"
#include "models/workflow/workflowmodel.h"
#include "models/workflow/scenariobuildermodel.h"
#include "models/workflow/errormodel.h"
#include "models/workflowerrormodel.h"
#include "models/visualattributemodel.h"
#include "models/tablemodel.h"
#include "models/layerinfoitem.h"
#include "models/catalogmapitem.h"
#include "models/columnmodel.h"
#include "models/graphmodel.h"
#include "models/chartmodel.h"
#include "models/consolescriptmodel.h"
#include "models/tabmodel.h"
#include "models/datapanemodel.h"
#include "models/objectcreator.h"
#include "models/ilwisobjectcreatormodel.h"
#include "models/workflow/scenariodesignermodel.h"
#include "ilwiscoreui/propertyeditors/numericrepresentationsetter.h"
#include "ilwiscoreui/propertyeditors/itemrepresentationsetter.h"
#include "ilwiscoreui/tableoperations/tableoperation.h"
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
        qmlRegisterType<WorkSpaceModel>("WorkSpaceModel",1,0,"WorkSpaceModel");
        qmlRegisterType<ResourceModel>("ResourceModel",1,0,"ResourceModel");
        qmlRegisterType<OperationCatalogModel>("OperationCatalogModel",1,0,"OperationCatalogModel");
        qmlRegisterType<OperationModel>("OperationModel",1,0,"OperationModel");
        qmlRegisterType<ApplicationFormExpressionParser>("ApplicationFormExpressionParser",1,0,"FormBuilder");
        qmlRegisterType<WorkflowMetadataFormBuilder>("WorkflowMetadataFormBuilder",1,0,"WorkflowMetadataFormBuilder");
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
        qmlRegisterType<NumericRepresentationSetter>("NumericRepresentationSetter", 1,0, "NumericRepresentationSetter");
        qmlRegisterType<ItemRepresentationSetter>("ItemRepresentationSetter", 1,0, "ItemRepresentationSetter");
        qmlRegisterType<RepresentationElement>("RepresentationElement", 1,0, "RepresentationElement");
        qmlRegisterType<ProjectionParameterModel>("ProjectionParameterModel", 1,0, "ProjectionParameterModel");
        qmlRegisterType<ScenarioBuilderModel>("ScenarioBuilderModel", 1,0, "ScenarioBuilderModel");
        qmlRegisterType<WorkflowModel>("WorkflowModel", 1,0, "WorkflowModel");
        qmlRegisterType<VisualAttributeModel>("VisualAttributeModel", 1,0,"VisualAttributeModel");
        qmlRegisterType<TableModel>("TableModel", 1,0,"TableModel");
        qmlRegisterType<Ilwis::Desktop::TableOperation>("TableOperation",1,0,"TableOperation");
        qmlRegisterType<ColumnModel>("ColumnModel", 1,0,"ColumnModel");
        qmlRegisterType<LayerInfoItem>("LayerInfoItem", 1,0,"LayerInfoItem");
        qmlRegisterType<CatalogMapItem>("CatalogMapItem", 1,0,"CatalogMapItem");
        qmlRegisterType<ChartModel>("ChartModel", 1,0,"ChartModel");
        qmlRegisterType<GraphModel>("GraphModel", 1,0,"GraphModel");
        qmlRegisterType<CatalogFilterModel>("CatalogFilterModel", 1,0,"CatalogFilterModel");
        qmlRegisterType<ConsoleLineModel>("ConsoleLineModel", 1,0,"ConsoleLineModel");
        qmlRegisterType<ConsoleScriptModel>("ConsoleScriptModel", 1,0,"ConsoleScriptModel");
        qmlRegisterType<DataPaneModel>("DataPaneModel", 1,0,"DataPaneModel");
        qmlRegisterType<TabModel>("TabModel", 1,0,"TabModel");
        qmlRegisterType<SidePanelModel>("SidePanelModel", 1,0,"SidePanelModel");
        qmlRegisterType<ObjectCreator>("ObjectCreator", 1,0,"ObjectCreator");
        qmlRegisterType<IlwisObjectCreatorModel>("IlwisObjectCreatorModel", 1,0,"IlwisObjectCreatorModel");
        qmlRegisterType<ScenarioDesignerModel>("ScenarioDesignerModel", 1,0,"ScenarioDesignerModel");
        qmlRegisterType<WorkflowErrorModel>("WorkflowErrorModel", 1,0,"WorkflowErrorModel");
        qmlRegisterType<ErrorModel>("ErrorModel", 1,0,"ErrorModel");

        MasterCatalogModel mastercatalogmodel(ctx);

        ApplicationFormExpressionParser formbuilder;
        WorkflowMetadataFormBuilder workflowmetadataformbuilder;
        UserMessageHandler messageHandler;
        OperationCatalogModel operations;
        TranquilizerHandler *tranquilizers = new TranquilizerHandler();
        ScenarioBuilderModel scenarios;
        DataPaneModel datapane;
        ObjectCreator objcreator;
        uicontext()->prepare();
        uicontext()->qmlContext(ctx);

        operations.prepare({"globaloperationscatalog", true});

        QThread *trqthread = new QThread;

        ctx->setContextProperty("mastercatalog", &mastercatalogmodel);
        ctx->setContextProperty("formbuilder", &formbuilder);
        ctx->setContextProperty("workflowmetadataformbuilder", &workflowmetadataformbuilder);
        ctx->setContextProperty("messagehandler", &messageHandler);
        ctx->setContextProperty("tranquilizerHandler", tranquilizers);
        ctx->setContextProperty("operations", &operations);
        ctx->setContextProperty("scenarios", &scenarios);
        ctx->setContextProperty("datapane", &datapane);
        ctx->setContextProperty("objectcreator", &objcreator);
        ctx->setContextProperty("uicontext", uicontext().get());


        mastercatalogmodel.connect(&operations, &OperationCatalogModel::updateCatalog,&mastercatalogmodel, &MasterCatalogModel::updateCatalog );
        operations.connect(uicontext().get(),&UIContextModel::currentWorkSpaceChanged, &operations, &OperationCatalogModel::workSpaceChanged);
        messageHandler.connect(kernel()->issues().data(), &IssueLogger::updateIssues,&messageHandler, &UserMessageHandler::addMessage );


        TranquilizerWorker *trw = new TranquilizerWorker;
        trw->moveToThread(trqthread);
        trqthread->connect(kernel(), &Kernel::updateTranquilizer, trw, &TranquilizerWorker::updateTranquilizer);
        trqthread->connect(kernel(), &Kernel::createTranquilizer, trw, &TranquilizerWorker::createTranquilizer);
        trqthread->connect(kernel(), &Kernel::removeTranquilizer, trw, &TranquilizerWorker::removeTranquilizer);
        trqthread->connect(trw, &TranquilizerWorker::sendUpdateTranquilizer, tranquilizers, &TranquilizerHandler::updateTranquilizer);
        trqthread->connect(trw, &TranquilizerWorker::sendCreateTranquilizer, tranquilizers, &TranquilizerHandler::createTranquilizer);
        trqthread->connect(trw, &TranquilizerWorker::sendRemoveTranquilizer, tranquilizers, &TranquilizerHandler::removeTranquilizer);

        engine.load("qml/DesktopClient.qml");

        QObject *topLevel = engine.rootObjects().value(0);
        uicontext()->rootObject(topLevel);
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

        trqthread->start();
        uicontext()->initializeDataPane();
        int ret =  app.exec();
        Ilwis::exitIlwis();

        return ret;
    }catch (const ErrorObject& err){
        qDebug() << err.message();
    }

    return 0;


}
