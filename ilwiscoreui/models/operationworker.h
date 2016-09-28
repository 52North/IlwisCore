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
    void operationResults(QVariantList values);

private:
   OperationExpression _expression;

};
}

#endif // OPERATIONWORKER_H
