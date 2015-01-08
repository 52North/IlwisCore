#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <QObject>
#include <QUrl>
#include "identity.h"
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT PropertyEditorMetaData : public QObject, public Ilwis::Identity
{
    Q_OBJECT
    Q_PROPERTY(QString editorName READ editorName CONSTANT)
    Q_PROPERTY(QString qmlUrl READ qmlUrl CONSTANT)
public:
    PropertyEditorMetaData(QObject *parent = 0);
    explicit PropertyEditorMetaData(const QString& name, const QUrl& Url, QObject *parent = 0);
    PropertyEditorMetaData(const PropertyEditorMetaData& metadata, QObject *parent);

private:
    QString editorName() const;
    QString qmlUrl() const;
    QUrl _qmlUrl;
signals:

public slots:
};

#endif // PROPERTYEDITOR_H
