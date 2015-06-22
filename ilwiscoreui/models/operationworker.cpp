#include <QCoreApplication>
#include "operationworker.h"
#include "raster.h"
#include "commandhandler.h"
#include "operation.h"
#include "ilwiscontext.h"

using namespace Ilwis;

OperationWorker::OperationWorker(const OperationExpression &opExpr) :_expression(opExpr)
{

}

void OperationWorker::process(){
    Operation op(_expression);
    SymbolTable tbl;
    ExecutionContext ctx;

    if(op->execute(&ctx, tbl)){
        if ( ctx._results.size() > 0){
            for(auto resultName : ctx._results){
                Symbol symbol = tbl.getSymbol(resultName);
                if ( hasType(symbol._type, itNUMBER)){
                    result += symbol._var.toDouble();
                }else if ( hasType(symbol._type, itSTRING)){
                    result += symbol._var.toString();
                }else if ( hasType(symbol._type, (itCOVERAGE | itTABLE))){
                    if ( symbol._type == itRASTER){
                        IRasterCoverage raster = symbol._var.value<IRasterCoverage>();
                        QString path = context()->persistentInternalCatalog().toString() + "/" + raster->name() + ".ilwis";
                        raster->connectTo(path,
                                          "rastercoverage",
                                          "stream",
                                          IlwisObject::cmOUTPUT);
                        raster->store();
                    }
                }
            }
        }
    }else {
        qDebug() << "operation failed";
    }
    emit finished();
}
