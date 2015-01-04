#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <QObject>
#include "identity.h"

class PropertyEditor : public QObject, public Ilwis::Identity
{
    Q_OBJECT
public:
    explicit PropertyEditor(const QString& name, QObject *parent = 0);
    ~PropertyEditor();

signals:

public slots:
};

#endif // PROPERTYEDITOR_H
