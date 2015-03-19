#ifndef MAPINFORMATIONPROPERTYSETTER_H
#define MAPINFORMATIONPROPERTYSETTER_H
#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class MapInformationPropertySetter : public VisualAttributeEditor
{
public:
    MapInformationPropertySetter(QObject *parent = 0);
    ~MapInformationPropertySetter();

    bool canUse(const Ilwis::IIlwisObject& obj, const ColumnDefinition &) const;
    bool canUse(const IIlwisObject &obj, const QString &name) const;
    static VisualAttributeEditor *create();

    void prepare(CoverageLayerModel *parentLayer, const Ilwis::IIlwisObject& bj, const ColumnDefinition &datadef = Ilwis::ColumnDefinition());

    NEW_PROPERTYEDITOR(MapInformationPropertySetter)
};

#endif // MAPINFORMATIONPROPERTYSETTER_H
