#include <QObject>
#include <QVector>
#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QUrlQuery>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "mastercatalog.h"


//----------------------------------
Ilwis::CommandHandler *Ilwis::CommandHandler::_commandHandler= 0;
using namespace Ilwis;

//-----------------------------------
void ExecutionContext::clear()
{
    _silent = false;
    _threaded = true;
    _results.clear();
}

ExecutionContext::ExecutionContext(bool threaded) : _silent(false), _threaded(threaded){
}

void ExecutionContext::addOutput(SymbolTable &tbl, const QVariant &var, const QString &nme, quint64 tp, const Resource& res)
{
    QString name =  nme == sUNDEF ? SymbolTable::newAnonym() : nme;
    tbl.addSymbol(name,_scope, tp, var);
    _results.push_back(name);
    if ( name.indexOf(INTERNAL_PREFIX) == -1 && res.isValid()) {
        mastercatalog()->addItems({res});
    }
}

Ilwis::CommandHandler* Ilwis::commandhandler() {
    if (Ilwis::CommandHandler::_commandHandler == 0) {
        Ilwis::CommandHandler::_commandHandler = new Ilwis::CommandHandler(kernel()->parent());
        //Ilwis::CommandHandler::_context->init();

    }
    return Ilwis::CommandHandler::_commandHandler;
}


//-------------------------------------------------------
CommandHandler::CommandHandler(QObject *parent) :
    QObject(parent)
{
}

CommandHandler::~CommandHandler(){
   _commands.clear();
}

bool CommandHandler::execute(const QString& command, ExecutionContext *ctx) {
    SymbolTable tbl;
    OperationExpression expr(command, tbl);
    quint64 id = findOperationId(expr);
    if ( id != i64UNDEF) {
        QScopedPointer<OperationImplementation> oper(create( expr));
        if ( !oper.isNull() && oper->isValid()) {
            return oper->execute(ctx, tbl);
        }
    }
    return false;
}

bool CommandHandler::execute(const QString &command, ExecutionContext *ctx, SymbolTable &symTable)
{
    OperationExpression expr(command, symTable);
    quint64 id = findOperationId(expr);
    if ( id != i64UNDEF) {
        QScopedPointer<OperationImplementation> oper(create( expr));
        if ( !oper.isNull() && oper->isValid()) {
            return oper->execute(ctx, symTable);
        }
    }
    return false;
}

OperationImplementation *CommandHandler::create(const OperationExpression &expr)  {
    quint64 id = findOperationId(expr);
    auto iter = _commands.find(id);
    if ( iter != _commands.end()) {
        OperationImplementation *oper = ((*iter).second(id, expr));
        return oper;
    }
    return 0;
}

void CommandHandler::addOperation(quint64 id, CreateOperation op)
{

    if ( id != i64UNDEF) {
        _commands[id] = op;
    }
}

quint64 CommandHandler::findOperationId(const OperationExpression& expr) const {

    QSqlQuery db(kernel()->database());
    QSqlQuery db2(kernel()->database());
    QString query = QString("select * from mastercatalog where resource like '%1%' ").arg(expr.metaUrl().toString());
    if (db.exec(query)) {
        while ( db.next()){
            quint64 itemid = db.value("itemid").toLongLong();
            query = QString("select * from catalogitemproperties where itemid=%1").arg(itemid);
            if (db2.exec(query)) {
                std::map<QString, QString> values;
                while ( db2.next()){
                    QSqlRecord rec = db2.record();
                    values[rec.value("propertyname").toString()] = rec.value("propertyvalue").toString();
                }
                QString parmcount = values["inparameters"];
                if ( !expr.matchesParameterCount(parmcount))
                    continue;
                bool found = true;
                for(int i=0; i < expr.parameterCount(); ++i) {
                    QString key = QString("pin_%1_type").arg(i+1);
                    IlwisTypes tpExpr = expr.parm(i).valuetype();
                    auto iter = values.find(key);
                    if ( iter == values.end()){
                        found = false;
                        break;
                    }
                    IlwisTypes tpMeta = (*iter).second.toULongLong();
                    if ( (tpMeta & tpExpr) == 0 && tpExpr != i64UNDEF) {
                        found = false;
                        break;
                    }

                }
                if ( found)
                    return itemid;
            }
        }
    }
    ERROR2(ERR_NO_INITIALIZED_2,"metadata",expr.name());
    return i64UNDEF;
}
