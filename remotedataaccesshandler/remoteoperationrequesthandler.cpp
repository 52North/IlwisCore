#include "remoteoperationrequesthandler.h"
#include <QHostAddress>
#include "raster.h"
#include "table.h"
#include "featurecoverage.h"
#include "juliantime.h"
#include "symboltable.h"
#include "ilwiscontext.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"

using namespace Ilwis;
using namespace RemoteDataAccess;


RemoteOperationRequestHandler::RemoteOperationRequestHandler()
{
}

void RemoteOperationRequestHandler::service(HttpRequest &request, HttpResponse &response)
{
    try{
    QMultiMap<QByteArray,QByteArray> parameters = request.getParameterMap();
    QMultiMap<QByteArray,QByteArray>::Iterator iter;
    if ( (iter = parameters.find("expression")) != parameters.end() ){
        QString expr = iter.value();
        OperationExpression operationexpr(expr);
        quint64 operationid = commandhandler()->findOperationId(operationexpr);
        if ( operationid != i64UNDEF){
            Resource metadata = mastercatalog()->id2Resource(operationid);
            QString operationname = metadata.name();
            QString outputs = metadata["outparameters"].toString();
            QStringList parts = outputs.split("|");
            QString outputNames;
            for(int i = 0; i < parts.size(); ++i){
                if ( outputNames != "")
                    outputNames += ",";
                quint64 mark = (quint64)(1e8 * (double)Time::now());
                QString outputName = QString("%1_%2_%3").arg(operationname).arg(i).arg(mark);
                outputNames += outputName;
            }
            QString parmlist;
            for(int i = 0; i < operationexpr.parameterCount(); ++i){
                if ( parmlist != "")    {
                    parmlist += ",";
                }
                Parameter parm = operationexpr.parm(i);
                if ( parm.pathType() == Parameter::ptREMOTE ){
                    QUrl urlExpr(parm.value());
                    int index = parm.value().lastIndexOf("/");
                    QString name = parm.value().mid(index + 1);
                    parmlist += QString("http://%1:%2/dataaccess?datasource=%3&ilwistype=ilwisobject&service=ilwisobjects").arg(urlExpr.host()).arg(urlExpr.port()).arg(name);
                }else if ( parm.pathType() == Parameter::ptLOCALOBJECT  ){
                    error(TR("No local filenames allowed in requested data"), response);
                    return;
                }
                else
                    parmlist += parm.value();
            }
            QString operationexpression = outputNames + "=" + metadata.name() + "(" + parmlist + ")";
            OperationExpression a(operationexpression);
            Operation localoperation(a);
            ExecutionContext ctx;
            SymbolTable tbl;
            if ( localoperation->execute(&ctx, tbl)){
                for(auto result : ctx._results){
                    Symbol sym = tbl.getSymbol(result);
                    QString internalUrl = context()->persistentInternalCatalog().toString() + "/" + result + ".ilwis4";
                    QString typeName;
                    IIlwisObject obj;
                    if ( hasType(sym._type, itFEATURE)){
                        obj = sym._var.value<Ilwis::IFeatureCoverage>();
                        typeName = "featurecoverage";
                    }else if ( sym._type == itRASTER){
                        obj = sym._var.value<Ilwis::IRasterCoverage>();
                        typeName = "rastercoverage";
                    }else if (hasType(sym._type, itTABLE)){
                        obj = sym._var.value<Ilwis::ITable>();
                        typeName = "table";
                    }else if (hasType(sym._type, itCATALOG)){
                        obj = sym._var.value<Ilwis::ICatalog>();
                        typeName = "catalog";
                    }else
                        continue;

                    obj->connectTo(internalUrl,typeName,"stream",IlwisObject::cmOUTPUT);
                    obj->store();
                    response.setHeader("Content-Type", qPrintable("text/plain"));
                    response.setHeader("Content-Disposition", qPrintable("attachment;filename=" + result + ".dataurl"));
                    QString ip = response.host()->localAddress().toString();
                    quint16 port = response.host()->localPort();
                    QString baseurl = "http://%1:%2/dataaccess?datasource=operationresult/%3&ilwistype=%4&service=ilwisobjects";
                    QString url = QString(baseurl).arg(ip).arg(port).arg(result).arg(typeName);
                    response.write(url.toLocal8Bit());
                }
            }


        }
    }
    } catch(ErrorObject& err){
        error(err.message(),response);
    }
}

HttpRequestHandler *RemoteOperationRequestHandler::create()
{
    return new RemoteOperationRequestHandler();
}
