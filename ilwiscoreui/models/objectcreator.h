#ifndef OBJECTCREATOR_H
#define OBJECTCREATOR_H

#include <QObject>
#include <QStringList>
#include <QQmlListProperty>
#include <vector>
#include "kernel.h"
#include "ilwisdata.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
class Resource;
class IlwisObject;
typedef IlwisData<IlwisObject> IIlwisObject;
}
class IlwisObjectCreatorModel;

class ILWISCOREUISHARED_EXPORT ObjectCreator : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList createableObjects READ createableObjects CONSTANT)
    Q_PROPERTY( QQmlListProperty<IlwisObjectCreatorModel> activeCreators READ activeCreators NOTIFY activeCreatorsChanged)

public:
    explicit ObjectCreator(QObject *parent = 0);
    ~ObjectCreator();


    Q_INVOKABLE void setActiveCreator(qint32 index);
    Q_INVOKABLE QString createObject(const QVariantMap& parms);

    QStringList createableObjects() const;
    QQmlListProperty<IlwisObjectCreatorModel> activeCreators() ;

signals:
    void activeCreatorsChanged();

public slots:

private:
    QList<IlwisObjectCreatorModel *> _activeCreators;
    QList<IlwisObjectCreatorModel *> _creators;


    QString createNumericDomain(const QVariantMap &parms);
    QString createItemDomain(const QVariantMap &parms);
    QString createGeoreference(const QVariantMap &parms);
    QString createProjectedCoordinateSystem(const QVariantMap &parms);
    QString createWorkflow(const QVariantMap &parms);
};

#endif // OBJECTCREATOR_H
