#ifndef OpenCVModule_H
#define OpenCVModule_H

#include <QObject>
#include <QLibrary>
#include "kernel.h"

namespace Ilwis {
namespace OpenCV {
class OpenCVModule : public Module
{
public:
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit OpenCVModule(QObject *parent = 0);
    virtual ~OpenCVModule();

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    void getOperations(QVector<ICommandInfo *>& ) const{}
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.opencvconnector" FILE  "opencvconnector.json")
private:


signals:
};
}
}

#endif // OpenCVModule_H
