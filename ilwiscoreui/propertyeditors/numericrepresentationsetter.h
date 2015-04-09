#ifndef NUMERICREPRESENTATIONSETTER_H
#define NUMERICREPRESENTATIONSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class ILWISCOREUISHARED_EXPORT RepresentationElement : public QObject, public Ilwis::Identity{
    Q_OBJECT

public:
    explicit RepresentationElement(QObject *parent=0);
    Q_PROPERTY(QColor color READ color CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)

    QColor color() const;

private:
    QColor _color;
};


class ILWISCOREUISHARED_EXPORT NumericRepresentationSetter : public VisualAttributeEditor
{
    Q_OBJECT

    Q_PROPERTY(QString representationName READ representationName NOTIFY rprNameChanged)
public:
    NumericRepresentationSetter(QObject *parent = 0);
    ~NumericRepresentationSetter();

    void prepare(CoverageLayerModel *parentLayer, const IIlwisObject &, const ColumnDefinition &);
    bool canUse(const IIlwisObject &obj, const ColumnDefinition &) const;
    static VisualAttributeEditor *create();

    QString representationName() const;

    Q_INVOKABLE QColor color(double frac);
    QQmlListProperty<RepresentationElement> representationElements();

    NEW_PROPERTYEDITOR(NumericRepresentationSetter)
signals:
    void rprNameChanged();

private:
    QList<RepresentationElement *> _rprElements;
    IRepresentation _representation;
};

#endif // NUMERICREPRESENTATIONSETTER_H
