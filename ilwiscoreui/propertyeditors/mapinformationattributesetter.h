#ifndef MAPINFORMATIONPROPERTYSETTER_H
#define MAPINFORMATIONPROPERTYSETTER_H
#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class MapInformationPropertySetter : public VisualAttributeEditor
{
    Q_OBJECT

       Q_PROPERTY(float showInfo READ showInfo WRITE setShowInfo NOTIFY showInfoChanged)
public:
    MapInformationPropertySetter(QObject *parent = 0);
    ~MapInformationPropertySetter();

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    static VisualAttributeEditor *create();

    void prepare(CoverageLayerModel *parentLayer, const Ilwis::IIlwisObject& bj, const ColumnDefinition &datadef = Ilwis::ColumnDefinition());
    bool showInfo() const;
    void setShowInfo(bool yesno);

    NEW_PROPERTYEDITOR(MapInformationPropertySetter)

signals:
    void showInfoChanged();

};

#endif // MAPINFORMATIONPROPERTYSETTER_H
