#include <QCoreApplication>
#include <QThread>
#include "operationworker.h"
#include "raster.h"
#include "table.h"
#include "commandhandler.h"
#include "operation.h"
#include "catalog.h"
#include "operationhelper.h"
#include "ilwiscontext.h"
#include "uicontextmodel.h"

using namespace Ilwis;

OperationWorker::OperationWorker(const OperationExpression &opExpr) :_expression(opExpr)
{
   connect(this, &OperationWorker::sendMessage, kernel(), &Kernel::acceptMessage);
}

void OperationWorker::run(const OperationExpression &expression){
    try {

        uicontext()->showLastGeneratedResult("");
        Operation op(expression);
        SymbolTable tbl;
        ExecutionContext ctx;
        if(op->execute(&ctx, tbl)){

            if ( ctx._results.size() > 0){
                for(auto resultName : ctx._results){
                    Symbol symbol = tbl.getSymbol(resultName);
                    if ( hasType(symbol._type, itINTEGER)){
                        QVariantList varList = symbol._var.value<QVariantList>();
                        QString num = QString::number(varList[0].toInt());
                        kernel()->issues()->log(num, IssueObject::itRESULT);
                    }else if ( hasType(symbol._type, itDOUBLE)){
                        QVariantList varList = symbol._var.value<QVariantList>();
                        QString num = QString::number(varList[0].toDouble());
                        kernel()->issues()->log(num, IssueObject::itRESULT);
                    }  else if ( hasType(symbol._type, itSTRING)){
                        QVariantList varList = symbol._var.value<QVariantList>();
                        kernel()->issues()->log(varList[0].toString(), IssueObject::itRESULT);
                    }else if ( hasType(symbol._type, itBOOL)){
                        QVariantList varList = symbol._var.value<QVariantList>();
                        kernel()->issues()->log(varList[0].toString(), IssueObject::itRESULT);
                    }
                    if ( hasType(symbol._type, itCOVERAGE | itTABLE) ){
                        IIlwisObject obj = OperationHelper::variant2ilwisobject(symbol._var, symbol._type);
                        if ( obj.isValid()){
                            QString lgr = QString("bigthing.newCatalog('itemid=%1',\'%2\',\'%3\', \'other\')").arg(obj->resource().id()).arg(TypeHelper::type2name(obj->ilwisType())).arg(obj->resource().url().toString());
                            uicontext()->showLastGeneratedResult(lgr);
                        }
                    }
                }
            }

            kernel()->issues()->log(QString(TR("Operation has executed succesfully")), IssueObject::itMessage);
        }else {
            qDebug() << "operation failed";
        }
    }catch(const ErrorObject& err){

    }

}

void OperationWorker::process(){
    QThread *thread = QThread::currentThread();
    QVariant var = thread->property("workingcatalog");
    if ( var.isValid()){
        kernel()->setTLS("workingcatalog", new QVariant(var));
    }
    run(_expression);

    QThread *current = QThread::currentThread();
    QVariant runvar = current->property("runparameters");
    if ( runvar.isValid()){
        QVariantMap values = runvar.toMap();
        bool ok;
        quint32 id = values["runid"].toUInt(&ok);
        if ( ok){
            QVariantMap mp;
            mp["runid"] = id;
            emit sendMessage("operation","stopped", mp );
        }
    }

    emit finished();
}
