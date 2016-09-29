#include <QCoreApplication>
#include <QThread>
#include "operationworker.h"
#include "raster.h"
#include "table.h"
#include "commandhandler.h"
#include "operation.h"
#include "catalog.h"
#include "ilwiscontext.h"

using namespace Ilwis;

OperationWorker::OperationWorker(const OperationExpression &opExpr) :_expression(opExpr)
{
}

void OperationWorker::process(){
    try {
        QThread *thread = QThread::currentThread();
        QVariant var = thread->property("workingcatalog");
        if ( var.isValid()){
            ICatalog cat = var.value<ICatalog>();
            kernel()->setTLS("workingcatalog", new QVariant(var));
        }
        Operation op(_expression);
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
                    }
                }
            }

            kernel()->issues()->log(QString(TR("Operation has executed succesfully")), IssueObject::itMessage);
        }else {
            qDebug() << "operation failed";
        }
        emit finished();
    }catch(const ErrorObject& err){

    }
    emit finished();
}
