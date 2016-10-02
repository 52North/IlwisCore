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
public:
    explicit ScriptModel(QObject *parent = 0);
    ScriptModel(const Ilwis::IScript &script, QObject *parent);

    Q_INVOKABLE void store() const;
    Q_INVOKABLE void storeAs(const QString& file);
    Q_INVOKABLE void setScriptfromFile(const QString& filename);
    QString text() const;
    void text(const QString& txt);



    Q_INVOKABLE QString run(const QString &txt);
signals:
    void textChanged();
public slots:

private:
    Ilwis::IScript _script;
};

#endif // SCRIPTMODEL_H
