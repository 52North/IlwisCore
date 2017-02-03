#include <QtPlugin>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "kernel.h"
#include "hydroflow_module.h"


using namespace Ilwis;
using namespace Hydroflow;

Hydroflow_module::Hydroflow_module(QObject *parent) :
    Module(parent, "Hydroflow", "iv40","1.0")
{
}

QString Hydroflow_module::getInterfaceVersion() const
{
    return "iv40";
}

void Hydroflow_module::prepare()
{
  kernel()->issues()->log("Loaded Hydroflow module", IssueObject::itMessage);
}

QString Hydroflow_module::name() const
{
    return "Hydroflow plugin";
}

QString Hydroflow_module::version() const
{
    return "1.0";
}
