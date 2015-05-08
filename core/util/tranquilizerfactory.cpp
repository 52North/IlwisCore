#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"
#include "tranquilizer.h"
#include "consoletranquilizer.h"
#include "tranquilizerfactory.h"

using namespace Ilwis;

std::map<int, CreateTranquilizer>  TranquilizerFactory::_creators;

TranquilizerFactory::TranquilizerFactory(): AbstractFactory("TranquilizerFactory","ilwis","Creates all tranquilizers" )
{

}

TranquilizerFactory::~TranquilizerFactory()
{

}

void TranquilizerFactory::registerTranquilizerType(int runMode, CreateTranquilizer createFunc)
{
    _creators[runMode] = createFunc;
}

bool TranquilizerFactory::prepare()
{
    registerTranquilizerType(rmEMPTY, EmptyTranquilizer::create);
    registerTranquilizerType(rmCOMMANDLINE,ConsoleTranquilizer::create);
    registerTranquilizerType(rmCOMMANDLINE | rmNOUI,ConsoleTranquilizer::create);

    return true;
}

