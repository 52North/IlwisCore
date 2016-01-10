#ifndef STARTILWIS_H
#define STARTILWIS_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <memory>

class ApplicationFormExpressionParser;
class WorkflowMetadataFormBuilder;
class UserMessageHandler;
class OperationCatalogModel;
class TranquilizerHandler;
class ScenarioBuilderModel;
class DataPaneModel;
class ObjectCreator;
class PreferencesModel;
class MasterCatalogModel;
class KeyFilter;
class QThread;

class StartIlwis : public QObject
{
    Q_OBJECT
public:
    explicit StartIlwis();
    explicit StartIlwis(QObject *parent, QQmlApplicationEngine *eng);

    Q_INVOKABLE void init();
    Q_INVOKABLE void initMainWindow();

    void stop();
signals:

public slots:

private:
    MasterCatalogModel *_mastercatalogmodel;
    ApplicationFormExpressionParser *_formbuilder;
    UserMessageHandler *_messageHandler;
    OperationCatalogModel *_operations;
    TranquilizerHandler *_tranquilizers;
    ScenarioBuilderModel *_scenarios;
    DataPaneModel *_datapane;
    ObjectCreator *_objcreator;
    PreferencesModel *_preferences;
    KeyFilter *_keys;

    QQmlApplicationEngine *_engine;
    QThread *_trqthread;

//    std::unique_ptr<MasterCatalogModel> _mastercatalogmodel;
//    std::unique_ptr<ApplicationFormExpressionParser> _formbuilder;
//    std::unique_ptr<WorkflowMetadataFormBuilder> workflowmetadataformbuilder;
//    std::unique_ptr<UserMessageHandler> _messageHandler;
//    std::unique_ptr<OperationCatalogModel> operations;
//    std::unique_ptr<TranquilizerHandler> tranquilizers;
//    std::unique_ptr<ScenarioBuilderModel> scenarios;
//    std::unique_ptr<DataPaneModel> datapane;
//    std::unique_ptr<ObjectCreator> objcreator;
//    std::unique_ptr<PreferencesModel> preferences;
//    std::unique_ptr<KeyFilter> _keys;
};


#endif // STARTILWIS_H
