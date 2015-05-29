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
    addLine("");
}

ConsoleScriptModel::~ConsoleScriptModel()
{

}

QQmlListProperty<ConsoleLineModel> ConsoleScriptModel::lines()
{
    return QQmlListProperty<ConsoleLineModel>(this, _lines);
}

ConsoleLineModel* ConsoleScriptModel::addLine(const QString &txt)
{
    _lines.push_back(new ConsoleLineModel(txt, this));
    _lines.back()->lineNumber(_lines.size());
    return _lines.back();
    emit linesChanged();
}

QString ConsoleScriptModel::run(const QString &txt)
{
    Ilwis::SymbolTable syms;
    Ilwis::ExecutionContext ctx;
    QString statement = QString("runpython(%1)").arg(txt);
    auto expr = Ilwis::OperationExpression(statement);
    Ilwis::Operation operation(expr);
    operation->execute(&ctx, syms);
    return "next" + QString::number(_lines.size());
}

int ConsoleScriptModel::numberOfLines() const
{
    return _lines.size();
}

//----------------------------------------------------------
ConsoleLineModel::ConsoleLineModel(QObject *parent) : QObject(parent)
{

}

ConsoleLineModel::ConsoleLineModel(const QString &txt, QObject *parent) : QObject(parent)
{
    _content = txt;
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
