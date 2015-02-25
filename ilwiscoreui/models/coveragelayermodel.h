#ifndef COVERAGELAYERMODEL_H
#define COVERAGELAYERMODEL_H

#include <QQmlListProperty>
#include "resourcemodel.h"
#include "propertyeditors/propertyeditor.h"
#include "drawers/drawerinterface.h"
#include "ilwiscoreui_global.h"

class LayerManager;
class PropertyEditor;

class ILWISCOREUISHARED_EXPORT CoverageLayerModel : public ResourceModel
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<PropertyEditor> propertyEditors READ propertyEditors NOTIFY propertyEditorChanged)


public:
    CoverageLayerModel();
    CoverageLayerModel(quint32 layerIndex, const Ilwis::Resource &resource, const QList<PropertyEditor *> &editors, Ilwis::Geodrawer::DrawerInterface *drawer, QObject *obj=0);

    Q_INVOKABLE PropertyEditor* propertyEditor(const QString& name);
    Ilwis::Geodrawer::DrawerInterface *drawer();



private:
    QQmlListProperty<PropertyEditor> propertyEditors();
    QList<PropertyEditor *> _propertyEditors;
    Ilwis::Geodrawer::DrawerInterface *_drawer = 0;

signals:
    void propertyEditorChanged();

};

typedef QQmlListProperty<CoverageLayerModel> CoverageLayerModelList;
Q_DECLARE_METATYPE(CoverageLayerModelList)

#endif // COVERAGELAYERMODEL_H
