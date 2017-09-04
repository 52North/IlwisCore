#ifndef SpreadSheetConnectorsModule_H
#define SpreadSheetConnectorsModule_H

#include <QObject>
#include <QLibrary>
#include "kernel.h"

namespace Ilwis {
namespace SpreadSheetConnectors {

class SpreadSheetConnectorsModule : public Module
{
public:
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit SpreadSheetConnectorsModule(QObject *parent = 0);
    virtual ~SpreadSheetConnectorsModule();

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    void getOperations(QVector<ICommandInfo *>& ) const{}
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.spreadsheetconnectorsconnectors" FILE  "spreadsheetconnectors.json")
private:


signals:
};
}
}

#endif // SpecializedConnectorsMODULE_H
