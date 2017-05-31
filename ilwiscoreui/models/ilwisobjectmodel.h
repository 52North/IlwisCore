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

namespace Ilwis {

class CalcRangesWorker : public QObject{
    Q_OBJECT
public:
    CalcRangesWorker(quint64 rasterid);

public slots:
    void process();

signals:
    void finished();

private:
   quint64 _rasterid;

};
}
class ILWISCOREUISHARED_EXPORT IlwisObjectModel : public ResourceModel
{
    Q_OBJECT
    Q_PROPERTY(QString isValid READ isValid CONSTANT)
    Q_PROPERTY(QString creationDate READ creationDate CONSTANT)
    Q_PROPERTY(QString modifiedDate READ modifiedDate CONSTANT)
    Q_PROPERTY(bool isReadonly READ readonly WRITE readonly NOTIFY readOnlyChanged)
    Q_PROPERTY(bool isSystemObject READ isSystemObject CONSTANT)
    Q_PROPERTY(bool isAnonymous READ isAnonymous CONSTANT)
    Q_PROPERTY(QString externalFormat READ externalFormat CONSTANT)
    Q_PROPERTY(bool externalReadOnly READ externalReadOnly CONSTANT)
    Q_PROPERTY(bool isCoverage READ isCoverage CONSTANT)
    Q_PROPERTY(QString valuetype READ valuetype CONSTANT)
    Q_PROPERTY(bool isProjected READ isProjectedCoordinateSystem CONSTANT)
    Q_PROPERTY(QString projectionInfo READ projectionInfo CONSTANT)
    Q_PROPERTY(QQmlListProperty<AttributeModel> attributes READ attributes CONSTANT)
    Q_PROPERTY(QQmlListProperty<DomainItemModel> domainitems READ domainitems CONSTANT)
    Q_PROPERTY(QQmlListProperty<ProjectionParameterModel> projectionItems READ projectionItems CONSTANT)
    Q_PROPERTY(QVariantList layerInfo READ layerInfo NOTIFY layerInfoChanged)
    Q_PROPERTY(QStringList quickProps READ quickProps CONSTANT)
    Q_PROPERTY(bool hasAttributes READ hasAttributes CONSTANT)


public:
    IlwisObjectModel();
    explicit IlwisObjectModel(const Ilwis::Resource &source, QObject *parent = 0);

    QString creationDate() const;
    QString modifiedDate() const;
    bool readonly() const;
    void readonly(bool yesno) const;
    QString externalFormat() const;
    bool externalReadOnly() const;
    bool isProjectedCoordinateSystem() const;
    bool isSystemObject() const;
    bool isCoverage() const;
    QString projectionInfo() const;
    bool isAnonymous() const;

    QStringList test101();
    QQmlListProperty<AttributeModel> attributes();
    QQmlListProperty<DomainItemModel> domainitems();
    QQmlListProperty<ProjectionParameterModel> projectionItems();
    QString valuetype() const;
    QString value2string(const QVariant& value, const QString& attrName="");
    Q_INVOKABLE QString rangeDefinition(bool defaultRange, bool calc, const QString &columnName);
    Q_INVOKABLE QString getProperty(const QString& propertyname) const;
    Q_INVOKABLE bool canUse(const QString& id);
    Q_INVOKABLE void setAttribute(const QString& attrname, const QString& value, const QString& extra="" );
    Q_INVOKABLE OperationModel *operation(const QString& id);
    Q_INVOKABLE CatalogModel *catalog(const QString& id);
    Q_INVOKABLE void unload();
    Q_INVOKABLE QString copy(const QString& newUrl, const QString& format, const QString& provider);
    Q_INVOKABLE void recalcLayers();


    bool isValid() const;
    Ilwis::IIlwisObject object() const;
    QVariantList layerInfo() const;

public slots:
    void recalcDone();
protected:
    QList<AttributeModel *> _attributes;
    void resetAttributeModel(const QString &attributeName);
    Ilwis::Resource &itemRef();
    const Ilwis::Resource &itemRef() const;
signals:
    void readOnlyChanged();
    void layerInfoChanged();
private slots:
    QString valueType() const;
private:
    mutable Ilwis::IIlwisObject _ilwisobject;
    QList<DomainItemModel *> _domainItems;
    QList<ProjectionParameterModel *> _projectionParmItems;
    QString pixSizeString() const;
    QString centerPixelLocation() const;
    QString parentDomain() const;
    QStringList quickProps() const;

    bool hasAttributes() const;
};

#endif // ILWISOBJECTMODEL_H
