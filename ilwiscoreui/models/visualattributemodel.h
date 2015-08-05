#ifndef VISUALATTRIBUTEMODEL_H
#define VISUALATTRIBUTEMODEL_H

#include <QQmlListProperty>
#include "attributemodel.h"
#include "ilwiscoreui_global.h"

class GlobalLayerModel;
namespace Ilwis{
class Representation;
typedef IlwisData<Representation> IRepresentation;

}
class VisualAttributeEditor;
class CoverageLayerModel;

class ILWISCOREUISHARED_EXPORT VisualAttributeModel : public AttributeModel
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<VisualAttributeEditor> propertyEditors READ propertyEditors NOTIFY propertyEditorChanged)

public:
    VisualAttributeModel();
    ~VisualAttributeModel();

    VisualAttributeModel(const Ilwis::ColumnDefinition& def, CoverageLayerModel *parentLayer, const Ilwis::IIlwisObject& obj);
    Ilwis::IRepresentation representation() const;
    void representation(const Ilwis::IRepresentation& rpr);
    CoverageLayerModel* layer() const;
    CoverageLayerModel* layer();

    Q_INVOKABLE VisualAttributeEditor *propertyEditor(const QString &name);

    static const QString LAYER_ONLY;
    static const QString GLOBAL_ONLY;

signals:
    void propertyEditorChanged();

protected:
    QList<VisualAttributeEditor *> _propertyEditors;

private:
    QQmlListProperty<VisualAttributeEditor> propertyEditors();
    Ilwis::IRepresentation _representation;
    CoverageLayerModel *_layer = 0;

};

class LayerAttributeModel : public VisualAttributeModel{
public:
    LayerAttributeModel(CoverageLayerModel *parentLayer, const Ilwis::IIlwisObject& obj);
    QString attributename() const;

};

class GlobalAttributeModel : public VisualAttributeModel{
public:
    GlobalAttributeModel(const QString& label, const QString& associatedEditor, GlobalLayerModel *parentLayer);
    QString attributename() const;

private:
    QString _label;
};

#endif // VISUALATTRIBUTEMODEL_H
