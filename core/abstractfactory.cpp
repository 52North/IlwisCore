#include <QString>
#include <QUrl>
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"
#include "identity.h"
#include "resource.h"

using namespace Ilwis;

//a comment

AbstractFactory::AbstractFactory(const QString& ty, const QString& sub="", const QString& desc) : _type(ty), _subtype(sub), _description(desc)
{
}

QString Ilwis::AbstractFactory::description() const
{
    return _description;
}

QString Ilwis::AbstractFactory::type() const
{
    return _type;
}

QString Ilwis::AbstractFactory::subtype() const
{
    return _subtype;
}

QString Ilwis::AbstractFactory::key() const
{
    return _subtype + "::" + _type;
}

bool AbstractFactory::prepare()
{
    return true;
}

bool AbstractFactory::canUse(const Resource &) const
{
    return true;
}
