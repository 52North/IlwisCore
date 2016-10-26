#include "kernel.h"
#include "applicationsetup.h"

using namespace Ilwis;

ModelApplication::ModelApplication()
{

}

void ModelApplication::store(QDataStream &stream)
{
    Identity::store(stream)    ;
}

void ModelApplication::load(QDataStream &stream)
{
    Identity::load(stream)    ;
}
