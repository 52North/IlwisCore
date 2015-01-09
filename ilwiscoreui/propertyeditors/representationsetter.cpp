#include "kernel.h"
#include "mastercatalog.h"
#include "representationsetter.h"

using namespace Ilwis;
RepresentationSetter::RepresentationSetter(QObject *parent) : QObject(parent)
{
    prepare();
}

void RepresentationSetter::prepare()
{
    QString query = "type=" + QString::number(itREPRESENTATION);
    _representations = mastercatalog()->select(query);
}

QStringList RepresentationSetter::rprNames() const
{
    QStringList names;
    for(auto resource : _representations){
        names.append(resource.name());
    }
    return names;
}
