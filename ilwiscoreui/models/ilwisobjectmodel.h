#ifndef ILWISOBJECTMODEL_H
#define ILWISOBJECTMODEL_H

#include <QQmlListProperty>
#include "kernel.h"
#include "ilwisdata.h"
#include "resourcemodel.h"
#include "attributemodel.h"
#include "domainitemmodel.h"
#include "projectionparametermodel.h"

class OperationModel;
class CatalogModel;

class ILWISCOREUISHARED_EXPORT IlwisObjectModel : public ResourceModel
{
    Q_OBJECT
    Q_PROPERTY(QString isValid READ isValid CONSTANT)
    Q_PROPERTY(QString creationDate READ creationDate CONSTANT)
    Q_PROPERTY(QString modifiedDate READ modifiedDate CONSTANT)
    Q_PROPERTY(bool isReadonly READ readonly WRITE readonly NOTIFY readOnlyChanged)
    Q_PROPERTY(bool isSystemObject READ isSystemObject CONSTANT)
    Q_PROPERTY(QString externalFormat READ externalFormat CONSTANT)
    Q_PROPERTY(bool externalReadOnly READ externalReadOnly CONSTANT)
    Q_PROPERTY(QString valuetype READ valuetype CONSTANT)
    Q_PROPERTY(bool isProjected READ isProjectedCoordinateSystem CONSTANT)
    Q_PROPERTY(QString projectionInfo READ projectionInfo CONSTANT)
    Q_PROPERTY(QQmlListProperty<AttributeModel> attributes READ attributes CONSTANT)
    Q_PROPERTY(QQmlListProperty<DomainItemModel> domainitems READ domainitems CONSTANT)
    Q_PROPERTY(QQmlListProperty<ProjectionParameterModel> projectionItems READ projectionItems CONSTANT)


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
    bool isSystemObject() const;
    QString projectionInfo() const;

    QStringList test101();
    QQmlListProperty<AttributeModel> attributes();
    QQmlListProperty<DomainItemModel> domainitems();
    QQmlListProperty<ProjectionParameterModel> projectionItems();
    QString valuetype() const;
    QString value2string(const QVariant& value, const QString& attrName="");
    Q_INVOKABLE QString rangeDefinition(bool defaultRange, bool calc, const QString &columnName);
    Q_INVOKABLE QString getProperty(const QString& propertyname);
    Q_INVOKABLE bool canUse(const QString& id);
    Q_INVOKABLE void setAttribute(const QString& attrname, const QString& value, const QString& extra="" );
    Q_INVOKABLE OperationModel *operation(const QString& id);
    Q_INVOKABLE CatalogModel *catalog(const QString& id);


    bool isValid() const;
    Ilwis::IIlwisObject object() const;

protected:
    QList<AttributeModel *> _attributes;
    void resetAttributeModel(const QString &attributeName);
signals:
    void readOnlyChanged();
private slots:
    QString valueType() const;
private:
    mutable Ilwis::IIlwisObject _ilwisobject;
    QList<DomainItemModel *> _domainItems;
    QList<ProjectionParameterModel *> _projectionParmItems;
    QString pixSizeString() const;
    QString centerPixelLocation() const;
    QString parentDomain() const;
};

#endif // ILWISOBJECTMODEL_H
