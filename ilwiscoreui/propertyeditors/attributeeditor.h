#ifndef PROPERTYEDITOR2_H
#define PROPERTYEDITOR2_H

#include <QObject>
#include <QUrl>
#include "identity.h"
#include "uicontextmodel.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
class ColumnDefintion;

}
class CoverageLayerModel;

class ILWISCOREUISHARED_EXPORT VisualAttributeEditor : public QObject, public Ilwis::Identity
{
    Q_OBJECT
    Q_PROPERTY(QString editorName READ editorName CONSTANT)
    Q_PROPERTY(QString qmlUrl READ qmlUrl CONSTANT)
    Q_PROPERTY(int defaultHeight READ defaultHeight CONSTANT)
    Q_PROPERTY(int layerIndex READ layerIndex CONSTANT)
    Q_PROPERTY(QString displayName READ displayName CONSTANT)

public:
    VisualAttributeEditor(QObject *parent = 0);
    explicit VisualAttributeEditor(const QString& name, const QString& displayName, const QUrl& url, QObject *parent = 0);
    VisualAttributeEditor(const VisualAttributeEditor& metadata, QObject *parent);
    QString editorName() const;
    virtual int defaultHeight() const;
    virtual bool canUse(const Ilwis::IIlwisObject &obj, const Ilwis::ColumnDefinition &datadef = Ilwis::ColumnDefinition()) const;
    virtual bool canUse(const Ilwis::IIlwisObject &obj, const QString &name) const;
    virtual void prepare(CoverageLayerModel *parentLayer, const Ilwis::IIlwisObject& bj, const Ilwis::ColumnDefinition &datadef = Ilwis::ColumnDefinition());
    int layerIndex() const;
    QString displayName() const;

    virtual void setlayer(quint32 index, CoverageLayerModel *model);
protected:
    CoverageLayerModel* layer() const;
    CoverageLayerModel* layer();

    QUrl _qmlUrl;
private:
    CoverageLayerModel *_layer = 0;
    quint32 _layerIndex = 0;
    QString _displayName;
    QString qmlUrl() const;

signals:
    void attributesChanged();

public slots:
};

#define NEW_PROPERTYEDITOR(name) \
    private: \
static int dummy_propertyeditor;

#define REGISTER_PROPERTYEDITOR(propertyname, name) \
    int name::dummy_propertyeditor = uicontext()->addPropertyEditor(propertyname, name::create);

#endif // PROPERTYEDITOR2_H
