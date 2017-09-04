#ifndef POSTGRESQLMODULE_H
#define POSTGRESQLMODULE_H

#include <QObject>
#include <QLibrary>
#include "kernel.h"

namespace Ilwis {
namespace Postgresql {
class PostgresqlModule : public Module
{
public:
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit PostgresqlModule(QObject *parent = 0);
    virtual ~PostgresqlModule();

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    void getOperations(QVector<ICommandInfo *>& ) const{}
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.postgresqlconnector" FILE  "postgresqlconnector.json")
private:


signals:
};
}
}

#endif // POSTGRESQLMODULE_H
