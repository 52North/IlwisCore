#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QObject>
#include <QVector>
#include <QVariant>
#include <map>
#include "Kernel_global.h"


namespace Ilwis {

class OperationExpression;
class OperationImplementation;

typedef std::function<OperationImplementation *(quint64 metaid, const OperationExpression&)> CreateOperation;

struct KERNELSHARED_EXPORT ExecutionContext {
    void clear();
    ExecutionContext();
    bool _silent;
    bool _threaded;
    QVector<QVariant> _results;
};

class KERNELSHARED_EXPORT CommandHandler : public QObject
{
    Q_OBJECT
public:
    friend KERNELSHARED_EXPORT CommandHandler* commandhandler();

    explicit CommandHandler(QObject *parent);
    ~CommandHandler();

    bool execute(const QString &command, ExecutionContext *ctx=0);
    void addOperation(quint64 id, CreateOperation op);
    OperationImplementation *create(const Ilwis::OperationExpression &expr);
    quint64 findOperationId(const OperationExpression &expr) const;

private:
    std::map<quint64, CreateOperation> _commands;
    static CommandHandler *_commandHandler;


signals:

public slots:

};

KERNELSHARED_EXPORT CommandHandler* commandhandler();
}

#endif // COMMANDHANDLER_H
