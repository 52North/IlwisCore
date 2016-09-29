#ifndef StreamMODULE_H
#define StreamMODULE_H

#include <QObject>
#include <QLibrary>
#include "kernel.h"

namespace Ilwis {
namespace PythonScript {
class PythonScriptModule : public Module
{
public:
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit PythonScriptModule(QObject *parent = 0);
    virtual ~PythonScriptModule();

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    void getOperations(QVector<ICommandInfo *>& ) const{}
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.pythonscript" FILE  "pythonscript.json")
private:


    void loadPythonOperations(quint64 id);
    void loadOperations(const QFileInfo &pyFileInfo, quint64 id);
    bool splitLine(QString line, QString &front, QString &back, bool toLower = false);
signals:
};
}
}

#endif // StreamMODULE_H
