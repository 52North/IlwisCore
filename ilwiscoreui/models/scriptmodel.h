#ifndef SCRIPTMODEL_H
#define SCRIPTMODEL_H

#include <QObject>

class ScriptModel : public QObject
{
    Q_OBJECT
public:
    explicit ScriptModel(QObject *parent = 0);

signals:

public slots:
};

#endif // SCRIPTMODEL_H