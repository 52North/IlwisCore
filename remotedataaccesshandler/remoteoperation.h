#ifndef REMOTEOPERATION_H
#define REMOTEOPERATION_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"

namespace Ilwis {

class SymbolTable;
class ExecutionContext;

namespace RemoteDataAccess {


class RemoteOperation : public QObject, public OperationImplementation
{
    Q_OBJECT
public:
    RemoteOperation();

    RemoteOperation(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

   NEW_OPERATION(RemoteOperation)
protected slots:
       void readReady();
       void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
       void error(QNetworkReply::NetworkError code);
       void finishedData();

private:
   QUrl _operationRequest;
   OperationExpression _operationexpr;
   QByteArray _bytes;
};
}
}

#endif // REMOTEOPERATION_H
