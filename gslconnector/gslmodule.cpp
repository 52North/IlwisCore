#include <QtPlugin>
#include <QScopedPointer>
#include <functional>
#include "kernel.h"

#include "kernel.h"
#include "raster.h"
#include "datadefinition.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"
#include "catalogconnectorfactory.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "gslmodule.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "dataformat.h"

using namespace Ilwis;
using namespace GSL;

GSLModule::GSLModule(QObject *parent) :
    Module(parent, "GSLModule", "iv40","1.0")
{
}

GSLModule::~GSLModule()
{
}

QString GSLModule::getInterfaceVersion() const
{
    return "iv40";

}

void GSLModule::prepare()
{
    kernel()->issues()->log("Loaded module Gnu Scientific Library (GSL) connector",IssueObject::itMessage);
}

QString GSLModule::getName() const
{
    return "GSL plugin";
}

QString GSLModule::getVersion() const
{
    return "1.0";
}






