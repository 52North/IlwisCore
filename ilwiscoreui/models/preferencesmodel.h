#ifndef PREFERENCESMODEL_H
#define PREFERENCESMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT PreferencesModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString cacheLocation READ cacheLocation WRITE setCacheLocation NOTIFY cacheLocationChanged)
    Q_PROPERTY(QString internalCatalog READ internalCatalog WRITE setInternalCatalog NOTIFY internalCatalogChanged)

public:
    explicit PreferencesModel(QObject *parent = 0);

private:
   QString cacheLocation() const;
   void setCacheLocation(const QString& loc);
   QString internalCatalog() const;
   void setInternalCatalog(const QString& loc);

signals:
    void cacheLocationChanged();
    void internalCatalogChanged();


public slots:
};

#endif // PREFERENCESMODEL_H
