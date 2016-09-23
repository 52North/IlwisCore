#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "uicontextmodel.h"
#include "consolescriptmodel.h"

ConsoleScriptModel::ConsoleScriptModel(QObject *parent) : QObject(parent)
{
}

ConsoleScriptModel::~ConsoleScriptModel()
{

}

QString ConsoleScriptModel::run(const QString &txt)
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

void ConsoleScriptModel::findExecutionLine(const QString &txt, int point)
{
    QString prevPart = txt.left(point);
    QStringList parts = prevPart.split("\n");
    _lastExecutionLine = parts.size();

}

void ConsoleScriptModel::countLines(const QString &txt)
{
    QStringList parts = txt.split("\n");
    _numberOfLines = parts.size();
}

int ConsoleScriptModel::numberOfLines() const
{
    return _numberOfLines;
}

int ConsoleScriptModel::lastExecutionLine() const
{
    return _lastExecutionLine;
}

