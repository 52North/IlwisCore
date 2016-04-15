#include <QCoreApplication>
#include "operationworker.h"
#include "raster.h"
#include "table.h"
#include "commandhandler.h"
#include "operation.h"
#include "ilwiscontext.h"

using namespace Ilwis;

OperationWorker::OperationWorker(const OperationExpression &opExpr) :_expression(opExpr)
{

}

void OperationWorker::process(){
    try {
        Operation op(_expression);
        SymbolTable tbl;
        ExecutionContext ctx;

        if(op->execute(&ctx, tbl)){
            if ( ctx._results.size() > 0){
                for(auto resultName : ctx._results){
                    Symbol symbol = tbl.getSymbol(resultName);
                    if ( hasType(symbol._type, itNUMBER)){
                        _result += symbol._var.toDouble();
                    }else if ( hasType(symbol._type, itSTRING)){
                        _result += symbol._var.toString();
                    }else if ( hasType(symbol._type, (itCOVERAGE | itTABLE))){
                        if ( symbol._type == itRASTER){
                            IRasterCoverage raster = symbol._var.value<IRasterCoverage>();
                            if ( raster.isValid())
                                _result = raster->resource().url().toString();
                        }else if(symbol._type == itTABLE){
                            ITable table = symbol._var.value<ITable>();
                            if(table.isValid())
                                _result = table->resource().url().toString();
                        }
                    }
                }
            }

            kernel()->issues()->log(QString(TR("Operation has executed succesfully")), IssueObject::itError);
        }else {
            qDebug() << "operation failed";
        }
        emit finished();
    }catch(const ErrorObject& err){

    }
    emit finished();
}
