#ifndef NUMERICREPRESENTATIONSETTER_H
#define NUMERICREPRESENTATIONSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"
#include "representationelement.h"

class ILWISCOREUISHARED_EXPORT NumericRepresentationSetter : public VisualAttributeEditor
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<RepresentationElement> representationElements READ representationElements NOTIFY itemsChanged)
public:
    NumericRepresentationSetter(QObject *parent = 0);
    ~NumericRepresentationSetter();

    void prepare(VisualAttributeModel *vattrib, const IIlwisObject &, const ColumnDefinition &);
    bool canUse(const IIlwisObject &obj, const ColumnDefinition &) const;
    static VisualAttributeEditor *create();

    Q_INVOKABLE QColor color(double frac);
    Q_INVOKABLE bool canUse(const QString& id) const;
    Q_INVOKABLE void setRepresentation(const QString& name)    ;
    QQmlListProperty<RepresentationElement> representationElements();

    NEW_PROPERTYEDITOR(NumericRepresentationSetter)
signals:
    void rprNameChanged();
    void itemsChanged();

private:

    IRepresentation _representation;
};

#endif // NUMERICREPRESENTATIONSETTER_H
