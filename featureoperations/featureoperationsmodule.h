#ifndef _H
#define _H

#include <QObject>
#include <QLibrary>
#include "module.h"
#include "kernel.h"

namespace Ilwis {
namespace FeatureOperations {

class FeatureOperationsModule  : public Module{
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit FeatureOperationsModule(QObject *parent = 0);

    QString getInterfaceVersion() const;
    QString name() const;
    QString version() const;
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.featureoperations" FILE  "featureoperations.json")


signals:

};
}
}

#endif // _H
