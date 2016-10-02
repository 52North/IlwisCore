#include "kernel.h"
#include "ilwisdata.h"
#include "operationmetadata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "commandhandler.h"
#include "operation.h"
#include "script.h"
#include "scriptmodel.h"
#include "mastercatalog.h"

ScriptModel::ScriptModel(QObject *parent) : ResourceModel()
{

}

ScriptModel::ScriptModel(const Ilwis::IScript& script,QObject *parent) : ResourceModel(script->resource(), parent)
{
    _script = script;
}

void ScriptModel::store() const
{
   if ( _script.isValid()){
       _script->store();
   }
}

void ScriptModel::storeAs(const QString &filename)
{
    Ilwis::Resource res(filename, itSCRIPT);
    Ilwis::mastercatalog()->addItems({res});
    QFile file(filename);
    if ( file.open(QFile::WriteOnly|QFile::Text)){
        if ( _script.isValid())
            file.write(_script->text().toLocal8Bit());
        else
            file.write("");
    }
    itemRef() = res;
    _script.prepare(res.id());

}

void ScriptModel::setScriptfromFile(const QString &filename)
{
    Ilwis::Resource res(filename, itSCRIPT);
    itemRef() = res;
    _script.prepare(res.id());

}

QString ScriptModel::text() const
{
    if ( _script.isValid()){
        return _script->text();
    }
    return "";
}

void ScriptModel::text(const QString &txt)
{
    if ( _script.isValid()){
        _script->text(txt);
    }
}

QString ScriptModel::run(const QString &txt)
{
    try{
        QString line = txt; // double qoutes messes things up in expression parsing so we replace them by single quotes
        line.replace("\"", "'");
        Ilwis::SymbolTable syms;
        Ilwis::ExecutionContext ctx;
        QString statement = QString("runpython(\"%1\")").arg(line);
        auto expr = Ilwis::OperationExpression(statement);
        Ilwis::Operation operation(expr);
        operation->execute(&ctx, syms);
        if ( ctx._results.size() > 0 ){
            QString res = syms.getValue(ctx._results[0]).toString();
            return res;
        }
        return "";

    } catch (const Ilwis::ErrorObject){

    }
    return "";
}
