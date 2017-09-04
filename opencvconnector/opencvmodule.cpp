#include "kernel.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"
#include "opencvmodule.h"

using namespace Ilwis;
using namespace OpenCV;

OpenCVModule::OpenCVModule(QObject *parent) : Module(parent, "OpenCVModule", "iv40","1.0")
{

}

OpenCVModule::~OpenCVModule()
{

}

QString OpenCVModule::getInterfaceVersion() const
{
      return "iv40";
}

QString OpenCVModule::getName() const
{
    return "OpenCV plugin";
}

QString OpenCVModule::getVersion() const
{
    return "1.0";
}

void OpenCVModule::prepare()
{
    kernel()->issues()->log("Loaded OpenCV module",IssueObject::itMessage);
}
