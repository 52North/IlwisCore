#ifndef OBJECTCREATOR_H
#define OBJECTCREATOR_H

#include <QObject>
#include <QStringList>
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT ObjectCreator : public QObject
{
    Q_OBJECT
public:
    explicit ObjectCreator(QObject *parent = 0);
    ~ObjectCreator();

    Q_INVOKABLE QStringList createableObjects() const;

signals:

public slots:
};

#endif // OBJECTCREATOR_H
