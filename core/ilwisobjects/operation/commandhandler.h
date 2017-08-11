#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QObject>
#include <QVector>
#include <QVariant>
#include <map>
#include "kernel_global.h"
#include "ilwis.h"
#include "symboltable.h"

namespace Ilwis {

class OperationExpression;
class OperationImplementation;
class Parameter;
class Resource;

typedef std::function<OperationImplementation *(quint64 metaid, const OperationExpression&)> CreateOperation;

struct KERNELSHARED_EXPORT ExecutionContext {
    void clear(bool resultsOnly=false);
    ExecutionContext(bool threaded=false);
    bool _silent = false;
    bool _threaded = false;
    bool _useAdditionalParameters = false;
    qint16 _scope=0;
    std::vector<QString> _results;
    std::map<QString, QVariant> _additionalInfo;
    QString _masterGeoref;
    QString _masterCsy;
    std::ostream *_out;
    void setOutput(SymbolTable &tbl, const QVariant &var, const QString &nme, quint64 tp, const Ilwis::Resource &resource, const QString &addInfo=sUNDEF);
    void addOutput(SymbolTable &tbl, const QVariant &var, const QString &nme, quint64 tp, const Resource &resource, const QString &addInfo=sUNDEF);

};

class KERNELSHARED_EXPORT CommandHandler : public QObject
{
    Q_OBJECT
public:
    friend KERNELSHARED_EXPORT CommandHandler* commandhandler();

    explicit CommandHandler(QObject *parent);
    ~CommandHandler();

    bool execute(const QString &command, ExecutionContext *ctx);
    bool execute(const QString &command, ExecutionContext *ctx, SymbolTable& symTable);
    void addOperation(quint64 id, CreateOperation op);
    OperationImplementation *create(const Ilwis::OperationExpression &expr);
    quint64 findOperationId(const OperationExpression &expr) const;
    bool parmIsValid(int index, Parameter parm, std::map<QString, QString> values) const;

private:
    std::map<quint64, CreateOperation> _commands;
    static CommandHandler *_commandHandler;


signals:

public slots:

};

KERNELSHARED_EXPORT CommandHandler* commandhandler();
}

#endif // COMMANDHANDLER_H
