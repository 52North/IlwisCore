#ifndef GSLModule_H
#define GSLModule_H

#include <QObject>
#include <QLibrary>
#include "module.h"
#include "kernel.h"


namespace Ilwis {
namespace GSL {

class GSLModule : public Module
{
public:
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit GSLModule(QObject *parent = 0);
    ~GSLModule();

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.GSLconnector" FILE  "gslconnector.json")

private:


signals:
};
}
}

#endif // GSLModule_H
