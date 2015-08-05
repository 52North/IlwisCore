#ifndef ITEMREPRESENTATIONSETTER_H
#define ITEMREPRESENTATIONSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class RepresentationElement;

class ILWISCOREUISHARED_EXPORT ItemRepresentationSetter : public VisualAttributeEditor
{
    Q_OBJECT


    Q_PROPERTY(QQmlListProperty<RepresentationElement> representationElements READ representationElements NOTIFY itemsChanged)
public:
    ItemRepresentationSetter(QObject *parent=0);

    void prepare(VisualAttributeModel *vattrib, const IIlwisObject &obj, const ColumnDefinition &coldef);
    bool canUse(const IIlwisObject &obj, const ColumnDefinition &) const;
    static VisualAttributeEditor *create();

    QQmlListProperty<RepresentationElement> representationElements();
    void attributesChanged(Raw index, const QVariantMap &values);
     void representationChanged(const IRepresentation& rpr);

    NEW_PROPERTYEDITOR(ItemRepresentationSetter)

signals:
    void rprNameChanged();
    void itemsChanged();





};

#endif // ITEMREPRESENTATIONSETTER_H
