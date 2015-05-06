#ifndef VISUALATTRIBUTEMODEL_H
#define VISUALATTRIBUTEMODEL_H

#include <QQmlListProperty>
#include "attributemodel.h"
#include "ilwiscoreui_global.h"

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

    Q_INVOKABLE VisualAttributeEditor *propertyEditor(const QString &name);

    static const QString LAYER_ONLY;

signals:
    void propertyEditorChanged();

protected:
    QList<VisualAttributeEditor *> _propertyEditors;

private:
    QQmlListProperty<VisualAttributeEditor> propertyEditors();

};

class LayerAttributeModel : public VisualAttributeModel{
public:
    LayerAttributeModel(CoverageLayerModel *parentLayer, const Ilwis::IIlwisObject& obj);
    QString attributename() const;

};

#endif // VISUALATTRIBUTEMODEL_H
