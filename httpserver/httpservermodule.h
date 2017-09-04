#ifndef ILWIS3MODULE_H
#define ILWIS3MODULE_H

#include <QObject>
#include "module.h"
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"

namespace Ilwis {
namespace HTTP {

class HttpServerModule : public Module
{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit HttpServerModule(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.httpserver" FILE  "httpserver.json")

signals:

public slots:

};
}
}

#endif // ILWIS3MODULE_H
