#ifndef OPERATIONWORKER_H
#define OPERATIONWORKER_H

#include <QObject>
#include <QThread>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"


namespace Ilwis {

class OperationWorker : public QObject{
    Q_OBJECT
public:
    OperationWorker(const OperationExpression& opExpr);

public slots:
    void process();

signals:
    void finished();

private:
   OperationExpression _expression;
   QString _result;

};
}

#endif // OPERATIONWORKER_H
