#include "kernel.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "connectorinterface.h"
#include "operationmetadata.h"
#include "script.h"

using namespace Ilwis;

Script::Script()
{

}

Script::Script(const Resource &resource): OperationMetaData(resource)
{

}

QString Script::text()
{
    if (!connector()->dataIsLoaded()) {
        connector()->loadData(this);
    }
    return _text;
}

void Script::text(const QString &txt)
{
    _text = txt;
}

IlwisTypes Script::ilwisType() const
{
    return itSCRIPT;
}
