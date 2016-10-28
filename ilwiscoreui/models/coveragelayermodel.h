#ifndef COVERAGELAYERMODEL_H
#define COVERAGELAYERMODEL_H

#include <QQmlListProperty>
#include "ilwisobjectmodel.h"
#include "propertyeditors/attributeeditor.h"
//#include "drawers/drawerinterface.h"
#include "drawers/rootdrawer.h"
#include "ilwiscoreui_global.h"

class LayerManager;
class VisualAttributeModelEditor;
class VisualAttributeModel;

namespace Ilwis {
class Coverage;
typedef IlwisData<Coverage> ICoverage;

}
class ILWISCOREUISHARED_EXPORT CoverageLayerModel : public IlwisObjectModel
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<VisualAttributeModel> visualAttributes READ visualAttributes NOTIFY visualAttributesChanged)
    Q_PROPERTY(int layerIndex READ layerIndex CONSTANT)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY onActiveChanged)
    Q_PROPERTY(int activeAttributeIndex READ getActiveAttributeIndex WRITE setActiveAttributeIndex NOTIFY onActiveAttributeIndexChanged)

public:
    CoverageLayerModel();
    CoverageLayerModel(quint32 layerIndex, const Ilwis::Resource &resource, Ilwis::Geodrawer::DrawerInterface *drawer, QObject *obj=0);

    virtual Ilwis::Geodrawer::DrawerInterface *drawer();
    virtual const Ilwis::Geodrawer::DrawerInterface *drawer() const;
    bool active() const;
    void setActive(bool yesno);
    int getActiveAttributeIndex() const;
    void setActiveAttributeIndex(int index);
    QString activeAttribute() const;
    QString visualAttributeByIndex(int index) const;
    bool showInfo() const;
    void showInfo(bool yesno);
    void updateAttribute(const QString& attrName, const QString& rprTxt);
    void drawer(Ilwis::Geodrawer::DrawerInterface *drw);
    Ilwis::ICoverage coverage() const;
    Ilwis::Geodrawer::RootDrawer *rootDrawer();
    const Ilwis::Geodrawer::RootDrawer *rootDrawer() const;

signals:
    void visualAttributesChanged();
    void onActiveChanged();
    void onActiveAttributeIndexChanged();

protected:
    void addVisualAttribute(VisualAttributeModel *attr);
private:
    quint32 layerIndex() const;
    VisualAttributeModel *visualAttribute(const QString& name);
    QQmlListProperty<VisualAttributeModel> visualAttributes();
    void fillAttributes();
    void resetVisualAttributeModel(const QString &attributeName);

    Ilwis::Geodrawer::DrawerInterface *_drawer = 0;
    Ilwis::Geodrawer::RootDrawer *_rootDrawer = 0;
    QList<VisualAttributeModel *> _visualAttributes;
    quint32 _layerIndex;
    int _activeAttribute = 0;
    bool _showInfo = true;




};

typedef QQmlListProperty<CoverageLayerModel> CoverageLayerModelList;
Q_DECLARE_METATYPE(CoverageLayerModelList)

#endif // COVERAGELAYERMODEL_H
