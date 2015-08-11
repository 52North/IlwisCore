#ifndef BACKGROUNDLAYEREDITOR_H
#define BACKGROUNDLAYEREDITOR_H


#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class BackgroundLayerEditor : public VisualAttributeEditor
{
    Q_OBJECT

public:
    BackgroundLayerEditor(QObject *parent = 0);

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    static VisualAttributeEditor *create();

    NEW_PROPERTYEDITOR(BackgroundLayerEditor)
};

#endif // BACKGROUNDLAYEREDITOR_H
