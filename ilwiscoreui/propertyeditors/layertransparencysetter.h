#ifndef LAYERTRANSPARENCYSETTER_H
#define LAYERTRANSPARENCYSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/propertyeditor.h"

class LayerTransparencySetter : public PropertyEditor
{
    Q_OBJECT

    Q_PROPERTY(float transparency READ transparency WRITE transparency NOTIFY transparencyChanged)
public:
    LayerTransparencySetter(QObject *parent = 0);
    ~LayerTransparencySetter();

    bool canUse(const IIlwisObject &obj) const;
    static PropertyEditor *create();

    float transparency() const;
    void transparency(float tr);


  NEW_PROPERTYEDITOR(LayerTransparencySetter)
signals:
      void transparencyChanged();

private:
};

#endif // LAYERTRANSPARENCYSETTER_H
