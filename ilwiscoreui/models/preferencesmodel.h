#ifndef PREFERENCESMODEL_H
#define PREFERENCESMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT PreferencesModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString cacheLocation READ cacheLocation WRITE setCacheLocation NOTIFY cacheLocationChanged)
    Q_PROPERTY(QString internalCatalog READ internalCatalog WRITE setInternalCatalog NOTIFY internalCatalogChanged)
    Q_PROPERTY(double uiScale READ uiScale WRITE uiScale NOTIFY uiScaleChanged)


public:
    explicit PreferencesModel(QObject *parent = 0);

    Q_INVOKABLE QString preferedDataFormat(const QString& type);
    Q_INVOKABLE void setPreferedDataFormat(const QString& type, const QString& name);


private:
   QString cacheLocation() const;
   void setCacheLocation(const QString& loc);
   QString internalCatalog() const;
   void setInternalCatalog(const QString& loc);
   double uiScale() const;
   void uiScale(double sc);


signals:
    void cacheLocationChanged();
    void internalCatalogChanged();
    void uiScaleChanged();


public slots:
};

#endif // PREFERENCESMODEL_H
