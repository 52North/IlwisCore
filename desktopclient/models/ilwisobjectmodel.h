#ifndef ILWISOBJECTMODEL_H
#define ILWISOBJECTMODEL_H

#include <QQmlListProperty>
#include "kernel.h"
#include "ilwisdata.h"
#include "resourcemodel.h"
#include "attributemodel.h"
#include "domainitemmodel.h"

class IlwisObjectModel : public ResourceModel
{
    Q_OBJECT
    Q_PROPERTY(QString isValid READ isValid CONSTANT)
    Q_PROPERTY(QString creationDate READ creationDate CONSTANT)
    Q_PROPERTY(QString modifiedDate READ modifiedDate CONSTANT)
    Q_PROPERTY(bool isReadonly READ readonly CONSTANT)
    Q_PROPERTY(QString externalFormat READ externalFormat CONSTANT)
    Q_PROPERTY(bool externalReadOnly READ externalReadOnly CONSTANT)
    Q_PROPERTY(QString valuetype READ valuetype CONSTANT)
    Q_PROPERTY(bool isProjected READ isProjectedCoordinateSystem CONSTANT)
    Q_PROPERTY(QString projectionInfo READ projectionInfo CONSTANT)
    Q_PROPERTY(QQmlListProperty<AttributeModel> attributes READ attributes CONSTANT)
    Q_PROPERTY(QQmlListProperty<DomainItemModel> domainitems READ domainitems CONSTANT)


public:
    IlwisObjectModel();
    explicit IlwisObjectModel(const Ilwis::Resource &source, QObject *parent = 0);

    QString creationDate() const;
    QString modifiedDate() const;
    bool readonly() const;
    void readonly(bool yesno) const;
    QString description() const;
    QString externalFormat() const;
    bool externalReadOnly() const;
    void description(const QString& desc) const;
    bool isProjectedCoordinateSystem() const;
    QString projectionInfo() const;

    QStringList test101();
    QQmlListProperty<AttributeModel> attributes();
    QQmlListProperty<DomainItemModel> domainitems();
    QString valuetype() const;
    Q_INVOKABLE QString rangeDefinition(bool defaultRange);
    Q_INVOKABLE QString getProperty(const QString& propertyname);

    bool isValid() const;

private slots:
    QString valueType() const;
private:
    Ilwis::IIlwisObject _ilwisobject;
    QList<AttributeModel *> _attributes;
    QList<DomainItemModel *> _domainItems;
    QString pixSizeString() const;
    QString centerPixelLocation() const;
    QString parentDomain() const;
};

#endif // ILWISOBJECTMODEL_H
