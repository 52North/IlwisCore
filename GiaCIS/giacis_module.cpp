#include <QtPlugin>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "kernel.h"
#include "giacis_module.h"


using namespace Ilwis;
using namespace GiaCIS;

GiaCIS_module::GiaCIS_module(QObject *parent) :
    Module(parent, "GiaCIS", "iv40","1.0")
{
}

QString GiaCIS_module::getInterfaceVersion() const
{
    return "iv40";
}

void GiaCIS_module::prepare()
{
  kernel()->issues()->log("Loaded GiaCIS module", IssueObject::itMessage);
}

QString GiaCIS_module::name() const
{
    return "GiaCIS plugin";
}

QString GiaCIS_module::version() const
{
    return "1.0";
}
