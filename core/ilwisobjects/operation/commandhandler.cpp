#include <QObject>
#include <QVector>
#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QUrlQuery>
#include <iostream>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "mastercatalog.h"


//----------------------------------
Ilwis::CommandHandler *Ilwis::CommandHandler::_commandHandler= 0;
using namespace Ilwis;

//-----------------------------------
void ExecutionContext::clear(bool resultsOnly)
{

    _results.clear();
    _additionalInfo.clear();
    if (!resultsOnly) {
        _masterCsy = sUNDEF;
        _masterGeoref = sUNDEF;
        _silent = false;
        _threaded = true;
        _out = &std::cout;
        _useAdditionalParameters = false;
    }
}

ExecutionContext::ExecutionContext(bool threaded) {
    clear();
    _threaded = threaded;
}

void ExecutionContext::setOutput(SymbolTable &tbl, const QVariant &var, const QString &nme, quint64 tp, const Resource& resource, const QString& addInfo)
{
    QString name =  nme == sUNDEF ? SymbolTable::newAnonym() : nme;
    tbl.addSymbol(name,_scope, tp, var);
    _results.clear();
    _results.push_back(name);
    if ( addInfo != sUNDEF)
        _additionalInfo[name] = addInfo;
    if ( name.indexOf(ANONYMOUS_PREFIX) == -1 && resource.isValid()) {
        mastercatalog()->addItems({resource});
    }
}

void ExecutionContext::addOutput(SymbolTable &tbl, const QVariant &var, const QString &nme, quint64 tp, const Resource& resource, const QString& addInfo)
{
    QString name =  nme == sUNDEF ? SymbolTable::newAnonym() : nme;
    tbl.addSymbol(name,_scope, tp, var);
    _results.push_back(name);
    if ( addInfo != sUNDEF)
        _additionalInfo[name] = addInfo;
    if ( name.indexOf(ANONYMOUS_PREFIX) == -1 && resource.isValid()) {
        mastercatalog()->addItems({resource});
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
    if ( command == "") // ignore empty commands
        return true;

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
    if ( command == "") // ignore empty commands
        return true;

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
    auto docommand = [&](const OperationExpression &expression)->OperationImplementation *{
        quint64 id = findOperationId(expression);
        auto iter = _commands.find(id);
        if ( iter != _commands.end()) {
            OperationImplementation *oper = ((*iter).second(id, expression));
            return oper;
        }
        return 0;
    };

    if ( expr.isRemote()){
        OperationExpression remoteexpr("remoteoperation('" + expr.toString(true) + "')");
        return docommand(remoteexpr);
    }
    return docommand(expr);

}

void CommandHandler::addOperation(quint64 id, CreateOperation op)
{

    if ( id != i64UNDEF) {
        _commands[id] = op;
    }
}

quint64 CommandHandler::findOperationId(const OperationExpression& expr) const {

    InternalDatabaseConnection db;
    InternalDatabaseConnection db2;
    QUrl url = expr.metaUrl(false);
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
                bool found = true;
                if (expr.inputIsKeyword()) {
                    const auto& keywordParameters = expr.getKeywordParameters();
                    for(auto iter = keywordParameters.begin(); iter != keywordParameters.end(); ++iter) {
                        Parameter parm = iter.value();
                        bool paramExists = false;
                        for(auto iter2 = values.begin(); iter2 != values.end(); ++iter2) {
                            QString key = (*iter2).first;
                            QString value = (*iter2).second;
                            if (iter.key() == value && key.endsWith("parm_name")) {
                                key = key.split("_")[1];
                                if (parmIsValid(key.toInt(), parm, values)) {
                                    // Delete this and the key
                                    values.erase(iter2);
                                    values.erase(QString("pin_%1_optional").arg(key));
                                    paramExists = true;
                                }
                                break;
                            }
                        }
                        if (!paramExists) {
                            found = false;
                            break;
                        }
                    }
                    if (found) { // Still correct?
                        for(auto iter = values.begin(); iter != values.end(); ++iter) {
                            if ((*iter).first.endsWith("optional") && (*iter).second == "false") {
                                found = false;
                                break;
                            }
                        }
                    }
                } else {
                    QString parmcount = values["inparameters"];
                    if ( !expr.matchesParameterCount(parmcount))
                        continue;
                    long index = parmcount.indexOf('+') != -1 ? parmcount.left(index).toUInt() : 10000;
                    for(long i=0; i < expr.parameterCount(); ++i) {
                        int n = min(i+1, index);
                        if (!parmIsValid(n, expr.parm(i), values)) {
                            found = false;
                            break;
                        }
                    }
                }
                if (found)
                    return itemid;
            }
        }
    }
    ERROR2(ERR_NO_INITIALIZED_2,"metadata",expr.name());
    return i64UNDEF;
}

bool CommandHandler::parmIsValid(int index, Parameter parm, std::map<QString, QString> values) const {
    QString key = QString("pin_%1_type").arg(index);
    IlwisTypes tpExpr = parm.valuetype();
    auto iter = values.find(key);
    if ( iter == values.end()){
        return false;
    }
    IlwisTypes tpMeta = (*iter).second.toULongLong();
    if ( tpMeta != itSTRING) { // string matches with all
        if (!( hasType(tpMeta, itDOUBLE) && hasType(tpExpr, itNUMBER))) {
            if (!( hasType(tpMeta, itINTEGER) && hasType(tpExpr, itINTEGER))) {
                if ( (tpMeta & tpExpr) == 0 && tpExpr != i64UNDEF) {
                    if ( tpExpr == itSTRING){
                        if (parm.value() == ""){ // empty parameters are seen as strings and are acceptable. at operation level it should be decided what to do with it
                            return true;
                        }else if ( parm.pathType() == Parameter::ptREMOTE){
                            // we can't know what this parameter type realy is, so we accept it as valid
                            // if it is incorrect the prepare of the operation will fail so no harm done
                            return true;
                        }
                    }
                    return false;
                }
            }
        }
    }
    return true;
}
