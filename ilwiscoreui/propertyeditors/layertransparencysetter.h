#ifndef LAYERTRANSPARENCYSETTER_H
#define LAYERTRANSPARENCYSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class LayerTransparencySetter : public VisualAttributeEditor
{
    Q_OBJECT

    Q_PROPERTY(float transparency READ transparency WRITE transparency NOTIFY transparencyChanged)
public:
    LayerTransparencySetter(QObject *parent = 0);
    ~LayerTransparencySetter();

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    static VisualAttributeEditor *create();

    float transparency() const;
    void transparency(float tr);


  NEW_PROPERTYEDITOR(LayerTransparencySetter)
signals:
      void transparencyChanged();

private:
};

#endif // LAYERTRANSPARENCYSETTER_H
