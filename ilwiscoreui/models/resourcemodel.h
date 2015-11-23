#ifndef RESOURCEMODEL_H
#define RESOURCEMODEL_H

#include <QObject>
#include <QFileInfo>
#include "catalog.h"
#include "ilwiscoreui_global.h"

//namespace Ilwis {
//namespace Desktopclient {

class ILWISCOREUISHARED_EXPORT ResourceModel : public QObject
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
    Q_PROPERTY(QString  description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString  dimensions READ dimensions CONSTANT)
    Q_PROPERTY(QString  displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString  iconPath READ iconPath CONSTANT)
    Q_PROPERTY(bool  isRoot READ isRoot CONSTANT)
    Q_PROPERTY(QString  id READ id CONSTANT)
    Q_PROPERTY(quint64 type READ type CONSTANT)
    Q_PROPERTY(QString typeName READ typeName CONSTANT)
    Q_PROPERTY(QString url READ url CONSTANT)
    Q_PROPERTY(QString container READ container CONSTANT)
    Q_PROPERTY(bool isSelected READ isSelected WRITE setIsSelected NOTIFY isSelectedChanged)



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
    bool isSelected() const;
    void setIsSelected(bool yesno);


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
    void setDescription(const QString& desc);
    QString url() const;
    QString container() const;
    QString iconPath() const;
    void iconPath(const QString& name);
    bool isRoot() const;
    QString id() const;


    Ilwis::Resource item() const;

    Q_INVOKABLE QString virtual getProperty(const QString& propertyname) const;
    Q_INVOKABLE void makeParent(QObject *item);
    Q_INVOKABLE bool hasExtendedType(const QString& tp) const;

    static QString iconPath(IlwisTypes tp) ;
protected:
    QString _displayName;
private:
    QString propertyTypeName(quint64 typ, const QString &propertyName) const;
    QString propertyName(const QString &property) const;
    QString proj42DisplayName(const QString &proj4Def) const;

    Ilwis::Resource _item;
    QString _imagePath;
    QString _iconPath;
    quint64 _type;
    bool _isRoot;
    bool _selected = false;
    bool _is3d = false;



signals:
    void displayNameChanged();
    void descriptionChanged();
    void isSelectedChanged();
public slots:

};
//}
//}


#endif // RESOURCEMODEL_H
