#ifndef REPRESENTATIONSETTER_H
#define REPRESENTATIONSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include <QColor>
#include "ilwiscoreui_global.h"
#include "identity.h"
#include "resource.h"
#include "propertyeditor.h"

class ILWISCOREUISHARED_EXPORT RepresentationElement : public QObject, public Ilwis::Identity{
    Q_OBJECT

public:
    explicit RepresentationElement(QObject *parent);
    Q_PROPERTY(QColor color READ color CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)

    QColor color() const;

private:
    QColor _color;
};

class ILWISCOREUISHARED_EXPORT RepresentationSetter : public PropertyEditor
{
    Q_OBJECT


public:
    static PropertyEditor *create();
    Q_INVOKABLE RepresentationSetter(QObject *parent = 0);
    Q_PROPERTY(QQmlListProperty<RepresentationElement> representationElements READ representationElements NOTIFY rprElementsChanged)

    QQmlListProperty<RepresentationElement> representationElements() ;

signals:
    void rprElementsChanged();

private:
     QList<RepresentationElement *> _rprElements;

     NEW_PROPERTYEDITOR(RepresentationSetter)
};

#endif // REPRESENTATIONSETTER_H
