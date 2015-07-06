#include <functional>
#include <future>
#include "kernel.h"
#include "baseoperationsmodule.h"


using namespace Ilwis;
using namespace BaseOperations;

BaseOperationsModule::BaseOperationsModule(QObject *parent) : Module(parent, "BaseOperations", "iv40","1.0")
{
}

QString BaseOperationsModule::getInterfaceVersion() const
{
    return "iv40";

}

void BaseOperationsModule::prepare()
{
    kernel()->issues()->log("Loaded module BaseOperations",IssueObject::itMessage);
}

QString BaseOperationsModule::name() const
{
    return "Base Operations plugin";
}

QString BaseOperationsModule::version() const
{
    return "1.0";
}
