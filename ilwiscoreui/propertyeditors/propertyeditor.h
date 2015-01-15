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
public:
    PropertyEditor(QObject *parent = 0);
    explicit PropertyEditor(const QString& name, const QUrl& url, QObject *parent = 0);
    PropertyEditor(const PropertyEditor& metadata, QObject *parent);
    QString editorName() const;

    void layer(CoverageLayerModel *model);
protected:
    CoverageLayerModel* layer();
    QUrl _qmlUrl;
private:
    CoverageLayerModel *_layer = 0;
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
