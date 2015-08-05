#ifndef LAYERTRANSPARENCYSETTER_H
#define LAYERTRANSPARENCYSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class LayerOpacitySetter : public VisualAttributeEditor
{
    Q_OBJECT

    Q_PROPERTY(float opacity READ opacity WRITE opacity NOTIFY opacityChanged)
public:
    LayerOpacitySetter(QObject *parent = 0);

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    static VisualAttributeEditor *create();

    float opacity() const;
    void opacity(float tr);


  NEW_PROPERTYEDITOR(LayerOpacitySetter)
signals:
      void opacityChanged();

private:
};

#endif // LAYERTRANSPARENCYSETTER_H
