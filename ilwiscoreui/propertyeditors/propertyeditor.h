#ifndef PROPERTYEDITOR2_H
#define PROPERTYEDITOR2_H

#include <QObject>
#include <QUrl>
#include "identity.h"
#include "uicontextmodel.h"
#include "ilwiscoreui_global.h"

class CoverageLayerModel;

class ILWISCOREUISHARED_EXPORT PropertyEditor : public QObject, public Ilwis::Identity
{
    Q_OBJECT
    Q_PROPERTY(QString editorName READ editorName CONSTANT)
    Q_PROPERTY(QString qmlUrl READ qmlUrl CONSTANT)
    Q_PROPERTY(int defaultHeight READ defaultHeight CONSTANT)
    Q_PROPERTY(int layerIndex READ layerIndex CONSTANT)
public:
    PropertyEditor(QObject *parent = 0);
    explicit PropertyEditor(const QString& name, const QUrl& url, QObject *parent = 0);
    PropertyEditor(const PropertyEditor& metadata, QObject *parent);
    QString editorName() const;
    virtual int defaultHeight() const;
    int layerIndex() const;

    virtual void setlayer(quint32 index, CoverageLayerModel *model);
protected:
    CoverageLayerModel* layer() const;
    CoverageLayerModel* layer();

    QUrl _qmlUrl;
private:
    CoverageLayerModel *_layer = 0;
    quint32 _layerIndex = 0;
    QString qmlUrl() const;
signals:

public slots:
};

#define NEW_PROPERTYEDITOR(name) \
    private: \
static int dummy_propertyeditor;

#define REGISTER_PROPERTYEDITOR(typeint, propertyname, name) \
    int name::dummy_propertyeditor = uicontext()->addPropertyEditor(typeint, propertyname, name::create);

#endif // PROPERTYEDITOR2_H
