#ifndef PROPERTYEDITOR2_H
#define PROPERTYEDITOR2_H

#include <QObject>
#include <QUrl>
#include "identity.h"
#include "uicontextmodel.h"
#include "ilwiscoreui_global.h"

class CoverageLayerModel;

class ILWISCOREUISHARED_EXPORT VisualAttributeEditor : public QObject, public Ilwis::Identity
{
    Q_OBJECT
    Q_PROPERTY(QString editorName READ editorName CONSTANT)
    Q_PROPERTY(QString qmlUrl READ qmlUrl CONSTANT)
    Q_PROPERTY(int defaultHeight READ defaultHeight CONSTANT)
    Q_PROPERTY(int layerIndex READ layerIndex CONSTANT)
    Q_PROPERTY(QString displayName READ displayName CONSTANT)
    Q_PROPERTY(QStringList attributes READ attributes NOTIFY attributesChanged)
    Q_PROPERTY(bool isAttributeDependent READ attributeDependent CONSTANT)
public:
    VisualAttributeEditor(QObject *parent = 0);
    explicit VisualAttributeEditor(const QString& name, const QString& displayName, const QUrl& url, QObject *parent = 0);
    VisualAttributeEditor(const VisualAttributeEditor& metadata, QObject *parent);
    QString editorName() const;
    virtual int defaultHeight() const;
    virtual bool canUse(const Ilwis::IIlwisObject& obj) const;
    virtual void prepare(const Ilwis::IIlwisObject& bj);
    int layerIndex() const;
    QString displayName() const;
    QStringList attributes() const;
    bool attributeDependent() const;

    virtual void setlayer(quint32 index, CoverageLayerModel *model);
protected:
    CoverageLayerModel* layer() const;
    CoverageLayerModel* layer();

    void setAttributeDependent(bool yesno);

    QUrl _qmlUrl;
private:
    CoverageLayerModel *_layer = 0;
    quint32 _layerIndex = 0;
    QString _displayName;
    QString qmlUrl() const;
    bool _attributeDependent = false;
    QStringList _attributes;
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
