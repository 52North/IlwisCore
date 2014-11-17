#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include <QObject>
#include <QFileInfo>
#include "catalog.h"

//namespace Ilwis {
//namespace Desktopclient {

class ResourceModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString imagePath READ imagePath CONSTANT)
    Q_PROPERTY(QString  size READ size CONSTANT)
    Q_PROPERTY(QString  domainName READ domainName CONSTANT)
    Q_PROPERTY(QString  domainType READ domainType CONSTANT)
    Q_PROPERTY(QString  coordinateSystemName READ coordinateSystemName CONSTANT)
    Q_PROPERTY(QString  coordinateSystemType READ coordinateSystemType CONSTANT)
    Q_PROPERTY(QString  geoReferenceName READ geoReferenceName CONSTANT)
    Q_PROPERTY(QString  geoReferenceType READ geoReferenceType CONSTANT)
    Q_PROPERTY(QString  description READ description CONSTANT)
    Q_PROPERTY(QString  dimensions READ dimensions CONSTANT)
    Q_PROPERTY(QString  displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString  iconPath READ iconPath CONSTANT)
    Q_PROPERTY(bool  isRoot READ isRoot CONSTANT)
    Q_PROPERTY(QString  id READ id CONSTANT)
    Q_PROPERTY(quint64 type READ type CONSTANT)
    Q_PROPERTY(QString typeName READ typeName CONSTANT)
    Q_PROPERTY(QString url READ url CONSTANT)



public:
    ResourceModel();
    explicit ResourceModel(const Ilwis::Resource &source, QObject *parent = 0);
    ResourceModel(const ResourceModel& model);
    ResourceModel& operator=(const ResourceModel& model);
    virtual ~ResourceModel();
    QString imagePath() const;
    quint64 type() const;
    QString typeName() const;
    QString name() const;
    void resource(const Ilwis::Resource &res);
    Ilwis::Resource resource() const;
    Ilwis::Resource &resourceRef();


    QString domainName() const;
    QString domainType() const;
    QString coordinateSystemName() const;
    QString coordinateSystemType() const;
    QString geoReferenceName() const;
    QString geoReferenceType() const ;
    QString size() const;
    virtual QString description() const;
    QString dimensions() const;
    QString displayName() const;
    void setDisplayName(const QString& name);
    QString url() const;
    QString iconPath() const;
    bool isRoot() const;
    QString id() const;


    Ilwis::Resource item() const;

    Q_INVOKABLE QString virtual getProperty(const QString& propertyname);

    static QString iconPath(IlwisTypes tp) ;
protected:
    QString _displayName;
private:
    QString propertyTypeName(quint64 typ, const QString &propertyName) const;
    QString propertyName(const QString &property) const;
    QString proj42DisplayName(const QString &proj4Def) const;

    Ilwis::Resource _item;
    QString _imagePath;
    quint64 _type;
    bool _isRoot;



signals:
    void displayNameChanged();
public slots:

};
//}
//}


#endif // RESOURCEMODEL_H
