#ifndef CATALOGFILTERMODEL_H
#define CATALOGFILTERMODEL_H

#include <QObject>
#include "kernel.h"

//namespace Ilwis {
//namespace Desktopclient {


class CatalogFilterModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool selectionState READ selectionState WRITE setSelectionState NOTIFY selectionStateChanged)
    Q_PROPERTY(QString catalogQuery READ catalogQuery WRITE setCatalogQuery NOTIFY catalogQueryChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(bool isReadOnly READ isReadOnly NOTIFY readOnlyChanged)

public:
    explicit CatalogFilterModel(QObject *parent = 0);
    explicit CatalogFilterModel(const QString &query, const QString &name, const QString &icon, bool state=true);

    bool selectionState() const;
    void setSelectionState(bool yesno);
    QString catalogQuery() const;
    void setCatalogQuery(const QString& stmt);
    QString name() const;
    void setName(const QString& nm);
    QString icon() const;
    void setIcon(const QString& icpath);
    bool isReadOnly() const;
    void setReadOnly(bool yesno);

private:
    bool _selectionState;
    QString _query;
    QString _name;
    QString _icon;
    bool _readonly;
    
signals:
    void selectionStateChanged(bool sel);
    void catalogQueryChanged(QString query);
    void nameChanged(QString name);
    void iconChanged(QString icon);
    void readOnlyChanged(bool ro);
    
public slots:
    
};
//}
//}

#endif // CATALOGFILTERMODEL_H
