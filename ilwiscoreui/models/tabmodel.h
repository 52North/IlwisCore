#ifndef TABMODEL_H
#define TABMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT TabModel : public QObject{
    Q_OBJECT

    Q_PROPERTY(QString componentUrl READ componentUrl CONSTANT)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(bool canSeparate READ canSeparate CONSTANT)
    Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(QString side READ side CONSTANT)
    Q_PROPERTY(quint32 id READ id() CONSTANT)
public:
    TabModel(QObject* parent=0);
    TabModel(const QString& url, const QString& componentUrl, QObject* parent);
    QString componentUrl() const;
    QString displayName() const;
    void setDisplayName(const QString& nm);
    bool canSeparate() const;
    void setSelected(bool yesno);
    bool selected() const;
    QString side() const;
    Q_INVOKABLE void selectTab();
    Q_INVOKABLE quint32 id() const;
signals:
   void displayNameChanged();
   void selectedChanged();

private:
   static quint32 _id;
    QString _componentUrl;
    QString _displayName;
    bool _canSeparate = false;
    bool _selected = false;
};

#endif // TABMODEL_H
