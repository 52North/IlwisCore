#ifndef CALCULATORMODULE_H
#define CALCULATORMODULE_H

#include <QObject>
#include <QLibrary>
#include "module.h"
#include "kernel.h"

namespace Ilwis {

class IlwisScriptModule  : public Module{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit IlwisScriptModule(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString name() const;
    QString version() const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.ilwisscript" FILE  "ilwisscript.json")


signals:

};
}

#endif // CALCULATOR_H
