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
#include "operationmetadata.h"
#include "script.h"
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
#include "models/workflow/modelbuilder.h"
#include "models/workflow/errormodel.h"
#include "models/workflowerrormodel.h"
#include "models/visualattributemodel.h"
#include "models/tablemodel.h"
#include "models/layerinfoitem.h"
#include "models/catalogmapitem.h"
#include "models/columnmodel.h"
#include "models/graphmodel.h"
#include "models/chartmodel.h"
#include "models/tabmodel.h"
#include "models/datapanemodel.h"
#include "models/objectcreator.h"
#include "models/ilwisobjectcreatormodel.h"
#include "models/preferencesmodel.h"
#include "models/internaldatabasemodel.h"
#include "ilwiscoreui/propertyeditors/numericrepresentationsetter.h"
#include "ilwiscoreui/propertyeditors/itemrepresentationsetter.h"
#include "ilwiscoreui/tableoperations/tableoperation.h"
#include "keyfilter.h"
#include "startilwis.h"
#include "scriptmodel.h"
#include "applicationmodel.h"
#include "analysispattern.h"
#include "modelbuilder.h"
#include "modeldesigner.h"
#include "workflow/analysismodel.h"
#include "workflow/conceptmodel.h"
#include "workflow/applicationmodelui.h"
#include "../core/buildnumber.h"

StartIlwis::StartIlwis()
{

}

StartIlwis::StartIlwis(QObject *parent, QQmlApplicationEngine *eng) : QObject(parent), _engine(eng)
{

}

void StartIlwis::init() {

    try{
        QQmlContext *ctx =_engine->rootContext();
        if(!Ilwis::initIlwis(Ilwis::rmDESKTOP)){
            exit(-1);
        }

        QFileInfo ilwisroot = context()->ilwisFolder();
        QString qmlpluginpath = ilwisroot.absoluteFilePath() + "/extensions/ui";

        _engine->addImportPath(qmlpluginpath);
        _engine->addPluginPath(qmlpluginpath);

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
        qmlRegisterType<DataPaneModel>("DataPaneModel", 1,0,"DataPaneModel");
        qmlRegisterType<TabModel>("TabModel", 1,0,"TabModel");
        qmlRegisterType<SidePanelModel>("SidePanelModel", 1,0,"SidePanelModel");
        qmlRegisterType<ObjectCreator>("ObjectCreator", 1,0,"ObjectCreator");
        qmlRegisterType<IlwisObjectCreatorModel>("IlwisObjectCreatorModel", 1,0,"IlwisObjectCreatorModel");
        qmlRegisterType<ModelBuilder>("ModelBuilder", 1,0,"ModelBuilder");
        qmlRegisterType<ModelDesigner>("ModelDesigner", 1,0,"ModelDesigner");
        qmlRegisterType<WorkflowErrorModel>("WorkflowErrorModel", 1,0,"WorkflowErrorModel");
        qmlRegisterType<ErrorModel>("ErrorModel", 1,0,"ErrorModel");
        qmlRegisterType<PreferencesModel>("PreferencesModel",1,0,"PreferencesModel");
        qmlRegisterType<InternalDatabaseModel>("InternalDatabaseModel",1,0,"InternalDatabaseModel");
        qmlRegisterType<ScriptModel>("ScriptModel",1,0,"ScriptModel");
        qmlRegisterType<ApplicationModelUI>("ApplicationModel",1,0,"ApplicationModel");
        qmlRegisterType<AnalysisModel>("AnalysisModel",1,0,"AnalysisModel");
        qmlRegisterType<ConceptModel>("ConceptModel",1,0,"ConceptModel");

        _mastercatalogmodel = new MasterCatalogModel(ctx);
        _formbuilder = new ApplicationFormExpressionParser();
        _messageHandler = new UserMessageHandler();
        _operations = new OperationCatalogModel();
        _tranquilizers =new TranquilizerHandler();
        _modelBuilder =new ModelBuilder();
        _datapane =new DataPaneModel();
        _objcreator =new ObjectCreator();
        _preferences =new PreferencesModel();
        _database = new InternalDatabaseModel();
        _trqthread = new QThread;

        ctx->setContextProperty("mastercatalog", _mastercatalogmodel);
        ctx->setContextProperty("formbuilder", _formbuilder);
        ctx->setContextProperty("messagehandler", _messageHandler);
        ctx->setContextProperty("tranquilizerHandler", _tranquilizers);
        ctx->setContextProperty("operations", _operations);
        ctx->setContextProperty("modelbuilder", modelbuilder().get());
        ctx->setContextProperty("datapane", _datapane);
        ctx->setContextProperty("objectcreator", _objcreator);
        ctx->setContextProperty("preferences",_preferences);
        ctx->setContextProperty("internaldatabase",_database);
        ctx->setContextProperty("uicontext", uicontext().get());

        uicontext()->prepare();
        uicontext()->qmlContext(ctx);
        _mastercatalogmodel->prepare();
        _operations->prepare({"globaloperationscatalog", true});

        qRegisterMetaType<IssueObject>("IssueObject");
        qRegisterMetaType<UrlSet>("UrlSet");


        _operations->connect(uicontext().get(),&UIContextModel::currentWorkSpaceChanged, _operations, &OperationCatalogModel::workSpaceChanged);
        _messageHandler->connect(kernel()->issues().data(), &IssueLogger::updateIssues,_messageHandler, &UserMessageHandler::addMessage );


        TranquilizerWorker *trw = new TranquilizerWorker;
        _trqthread->setProperty("workingcatalog", qVariantFromValue(context()->workingCatalog()));
        trw->moveToThread(_trqthread);
        _trqthread->connect(kernel(), &Kernel::updateTranquilizer, trw, &TranquilizerWorker::updateTranquilizer);
        _trqthread->connect(kernel(), &Kernel::createTranquilizer, trw, &TranquilizerWorker::createTranquilizer);
        _trqthread->connect(kernel(), &Kernel::removeTranquilizer, trw, &TranquilizerWorker::removeTranquilizer);
        _trqthread->connect(trw, &TranquilizerWorker::sendUpdateTranquilizer, _tranquilizers, &TranquilizerHandler::updateTranquilizer);
        _trqthread->connect(trw, &TranquilizerWorker::sendCreateTranquilizer, _tranquilizers, &TranquilizerHandler::createTranquilizer);
        _trqthread->connect(trw, &TranquilizerWorker::sendRemoveTranquilizer, _tranquilizers, &TranquilizerHandler::removeTranquilizer);

        _trqthread->start();


        _keys = new KeyFilter() ;
        _mastercatalogmodel->connect(_keys, &KeyFilter::keyPressed, _mastercatalogmodel, &MasterCatalogModel::keyPressed);
        _mastercatalogmodel->connect(_keys, &KeyFilter::keyReleased, _mastercatalogmodel, &MasterCatalogModel::keyReleased);
        QCoreApplication::instance()->installEventFilter(_keys);
    } catch(ErrorObject&){

    } catch(...){

    }
}

void StartIlwis::initMainWindow()
{
    QObject *top =_engine->rootObjects().value(0);
    QObject *topLevel = top->findChild<QObject *>("mainwindow__mainui");
    if ( topLevel){
        uicontext()->rootObject(topLevel);
        QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
        window->setIcon(QIcon("./qml/images/ilwis4.png"));
        window->setTitle(" ");
        if ( !window ) {
            qWarning("Error: Your root item has to be a Window.");
            return;
        }
        window->show();
    }

    uicontext()->initializeDataPane();
}

QString StartIlwis::buildType() const
{
#ifdef ILWIS_VERSION_TYPE
    return ILWIS_VERSION_TYPE;
#endif
    return "";
}

QString StartIlwis::verionNumber() const
{
#ifdef ILWIS_VERSION_NUMBER
    return QString::number(ILWIS_VERSION_NUMBER);
#endif
return "";
}

QString StartIlwis::buildNumber() const
{
#ifdef ILWIS_BUILDNR
    return QString::number(ILWIS_BUILDNR);
#endif
    return "";
}

void StartIlwis::stop()
{

    Ilwis::exitIlwis();
    delete _mastercatalogmodel;
    delete _formbuilder;
    delete _messageHandler;
    delete _operations;
    delete _tranquilizers;
    delete _modelBuilder;
    delete _datapane;
    delete _objcreator;
    delete _preferences;
    delete _keys;
    _trqthread->quit();
    _trqthread->deleteLater();

}

