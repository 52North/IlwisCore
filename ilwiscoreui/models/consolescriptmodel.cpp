#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "consolescriptmodel.h"

ConsoleScriptModel::ConsoleScriptModel(QObject *parent) : QObject(parent)
{
    addLine("",false);
}

ConsoleScriptModel::~ConsoleScriptModel()
{

}

QQmlListProperty<ConsoleLineModel> ConsoleScriptModel::lines()
{
    return QQmlListProperty<ConsoleLineModel>(this, _lines);
}

ConsoleLineModel* ConsoleScriptModel::addLine(const QString &txt, bool readonly)
{
    _lines.push_back(new ConsoleLineModel(txt, readonly, this));
    _lines.back()->lineNumber(_lines.size());
    return _lines.back();
    emit linesChanged();
}

QString ConsoleScriptModel::run(const QString &txt)
{
    try{
        _lines[_lines.size() - 1] = new ConsoleLineModel(txt, true, this);
        _lines.back()->lineNumber(_lines.size());
        Ilwis::SymbolTable syms;
        Ilwis::ExecutionContext ctx;
        QString statement = QString("runpython(\"%1\")").arg(txt);
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

int ConsoleScriptModel::numberOfLines() const
{
    return _lines.size();
}

//----------------------------------------------------------
ConsoleLineModel::ConsoleLineModel(QObject *parent) : QObject(parent)
{

}

ConsoleLineModel::ConsoleLineModel(const QString &txt, bool readonly, QObject *parent) : QObject(parent)
{
    _content = txt;
    _readonly = readonly;
}

QString ConsoleLineModel::content() const
{
    return _content;
}

void ConsoleLineModel::setContent(const QString &txt)
{
    _content = txt;
}

int ConsoleLineModel::lineNumber() const
{
    return _lineNumber;
}

void ConsoleLineModel::lineNumber(int l)
{
    _lineNumber = l;
}

void ConsoleLineModel::readOnly(bool yesno)
{
    _readonly = yesno;
}

bool ConsoleLineModel::readOnly() const
{
    return _readonly;
}
