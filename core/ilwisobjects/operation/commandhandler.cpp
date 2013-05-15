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

ExecutionContext::ExecutionContext() : _silent(false), _threaded(true){
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
    OperationExpression expr(command);
    quint64 id = findOperationId(expr);
    if ( id != i64UNDEF) {
        OperationImplementation *oper = create( expr);
        if ( oper && oper->isValid())
            return oper->execute(ctx);
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
   //QString query = QString("select * from mastercatalog where resource like '%1%s' and (type & %2) != 0").arg(expr.metaUrl().toString()).arg(itOPERATIONMETADATA);
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
                int n = values["inparameters"].toInt();
                if ( n != expr.parameterCount())
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
                    IlwisTypes tpMeta = (*iter).second.toLongLong();
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
