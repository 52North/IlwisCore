#ifndef OBJECTCREATOR_H
#define OBJECTCREATOR_H

#include <QObject>
#include <QStringList>
#include <QQmlListProperty>
#include <vector>
#include "ilwiscoreui_global.h"

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

    QStringList createableObjects() const;
    QQmlListProperty<IlwisObjectCreatorModel> activeCreators() ;

signals:
    void activeCreatorsChanged();

public slots:

private:
    QList<IlwisObjectCreatorModel *> _activeCreators;
    QList<IlwisObjectCreatorModel *> _creators;


};

#endif // OBJECTCREATOR_H
