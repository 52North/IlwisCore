#ifndef SCRIPTMODEL_H
#define SCRIPTMODEL_H

#include <QObject>
#include "kernel.h"
#include "ilwisdata.h"
#include "resourcemodel.h"
#include "operationmetadata.h"
#include "script.h"

class ILWISCOREUISHARED_EXPORT ScriptModel : public ResourceModel
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE text NOTIFY textChanged)
    Q_PROPERTY(bool isDefaultConsole READ isDefaultConsole CONSTANT)
public:
    explicit ScriptModel(QObject *parent = 0);
    ScriptModel(const Ilwis::IScript &script, QObject *parent);
    ScriptModel(const Ilwis::IOperationMetaData &metadata, QObject *parent);

    Q_INVOKABLE void store() const;
    Q_INVOKABLE QString storeAs(const QString& file);
    Q_INVOKABLE void setScriptfromFile(const QString& filename);
    QString text() const;
    void text(const QString& txt);
    bool isDefaultConsole() const;



    Q_INVOKABLE QString run(const QString &txt);
signals:
    void textChanged();
public slots:

private:
    Ilwis::IScript _script;
    Ilwis::IOperationMetaData _metadata;
};

#endif // SCRIPTMODEL_H
