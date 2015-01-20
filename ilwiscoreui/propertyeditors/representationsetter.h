#ifndef REPRESENTATIONSETTER_H
#define REPRESENTATIONSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include <QColor>
#include "ilwiscoreui_global.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "representation.h"
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

    Q_PROPERTY(QString activeValueType READ activeValueType CONSTANT)
    Q_PROPERTY(QQmlListProperty<RepresentationElement> representationElements READ representationElements NOTIFY rprElementsChanged)
    Q_PROPERTY(QString representationName READ representationName NOTIFY rprNameChanged)
public:
    static PropertyEditor *create();
    Q_INVOKABLE RepresentationSetter(QObject *parent = 0);
    Q_INVOKABLE QColor color(double frac);
    Q_INVOKABLE QColor name2color(const QString &clr) const;

    QQmlListProperty<RepresentationElement> representationElements() ;
    QString activeValueType() const;
    QString representationName() const;
    void setlayer(CoverageLayerModel *model);

signals:
    void rprElementsChanged();
    void rprNameChanged();

private:
     QList<RepresentationElement *> _rprElements;
     IRepresentation _representation;

     NEW_PROPERTYEDITOR(RepresentationSetter)
};

#endif // REPRESENTATIONSETTER_H
