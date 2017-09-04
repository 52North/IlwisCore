#ifndef StreamMODULE_H
#define StreamMODULE_H

#include <QObject>
#include <QLibrary>
#include "kernel.h"

namespace Ilwis {
namespace Stream {
class StreamModule : public Module
{
public:
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit StreamModule(QObject *parent = 0);
    virtual ~StreamModule();

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    void getOperations(QVector<ICommandInfo *>& ) const{}
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.streamconnector" FILE  "streamconnector.json")

private:
    static IlwisTypes ilwisType(const QString &name);

signals:
};
}
}

#endif // StreamMODULE_H
