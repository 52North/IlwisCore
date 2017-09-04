#include <QEventLoop>
#include "remoteoperation.h"
#include "catalog.h"
#include "ilwiscontext.h"

using namespace Ilwis;
using namespace RemoteDataAccess;

REGISTER_OPERATION(RemoteOperation)

RemoteOperation::RemoteOperation()
{
}

RemoteOperation::RemoteOperation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool RemoteOperation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    QString ss = _operationRequest.toString();
    QNetworkRequest request(_operationRequest);

    QNetworkReply *reply = kernel()->network().get(request);

    connect(reply, &QNetworkReply::downloadProgress, this, &RemoteOperation::downloadProgress);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &RemoteOperation::error);
    connect(reply, &QNetworkReply::finished, this, &RemoteOperation::finishedData);
    connect(reply, &QNetworkReply::readyRead, this, &RemoteOperation::readReady);

    QEventLoop loop; // waits for request to complete
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    delete reply;

    return true;
}

void RemoteOperation::readReady()
{

    if (QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender())) {
        _bytes.append(reply->readAll());
    }
}

void RemoteOperation::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);
}

void RemoteOperation::error(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    if (QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender())) {
        qErrnoWarning("Error!");

        reply->deleteLater();
    }
}

void RemoteOperation::finishedData()
{
}

Ilwis::OperationImplementation *RemoteOperation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RemoteOperation(metaid, expr);
}

Ilwis::OperationImplementation::State RemoteOperation::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString expr = _expression.input<QString>(0);
    if ( expr == "")
        return sPREPAREFAILED;

    QString operation = expr.mid(1,expr.size() - 3);
    _operationexpr = OperationExpression(operation);
    QString workingcatalog = context()->workingCatalog()->resource().url().toString();
    QString urlexpr;
    bool startserver = false;
    for(int i = 0; i < _operationexpr.parameterCount(); ++i){
        if ( urlexpr != "")    {
            urlexpr += ",";
        }
        Parameter parm = _operationexpr.parm(i);
        if ( parm.pathType() == Parameter::ptLOCALOBJECT ){
            int index = parm.value().lastIndexOf("/");
            QString name = parm.value().mid(index);
            urlexpr += QString("http://%1:%2/dataaccess/%3").arg(context()->ipv4()).arg(ilwisconfig("server-settings/port", 8080)).arg(name);
            startserver = true;
        }else if ( parm.pathType() == Parameter::ptNONE && !context()->workingCatalog()->isRemote() ){
            urlexpr += QString("http://%1:%2/dataaccess/%3").arg(context()->ipv4()).arg(ilwisconfig("server-settings/port", 8080)).arg(parm.value());
            startserver = true;
        }
        else
            urlexpr += parm.value();
    }
    QUrl rightside(_operationexpr.toString(true));
    QString path = rightside.path().mid(1);
    int index = path.indexOf("(");
    if ( index == -1){
        ERROR2(ERR_ILLEGAL_VALUE_2,TR("operation definition"),_operationexpr.toString());
        return sPREPAREFAILED;
    }
    QString name = path.left(index);
    path = name + "(" + urlexpr + ")";
    _operationRequest = QUrl(QString("http://%1:%2/operation?expression=%3").arg(rightside.host()).arg(rightside.port()).arg(path));



    if (!_operationRequest.isValid())
        return sPREPAREFAILED;
    if ( startserver){
        try {
            QString expr="httpserver(8095)";
            QString datafolder = Ilwis::ilwisconfig("remotedataserver/root-data-folder", QString("?"));

            Ilwis::ExecutionContext ctx;
            Ilwis::SymbolTable syms;
            if (!Ilwis::commandhandler()->execute(expr,&ctx,syms)){
                ERROR1(ERR_OPERATION_FAILID1,TR("starting http server"));
                return sPREPAREFAILED;
            }
        } catch(const ErrorObject& err){
            ERROR1(ERR_OPERATION_FAILID1, err.message());
            return sPREPAREFAILED;
        }
    }
    return sPREPARED;
}

quint64 RemoteOperation::createMetadata()
{
    OperationResource operation({"ilwis://operations/remoteoperation"});
    operation.setSyntax("remoteoperation(expression");
    operation.setDescription(TR("executes remotely an operation on a server machine. results are either passed direcly back to the client or are stored for later retrieval"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itSTRING , TR("expression"),TR("expression to be executed"));
    operation.setOutParameterCount({0});
    operation.setKeywords("server, remote");

    mastercatalog()->addItems({operation});
    return operation.id();
}
