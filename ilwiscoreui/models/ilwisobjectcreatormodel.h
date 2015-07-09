#ifndef IWISOBJECTCREATORMODEL_H
#define IWISOBJECTCREATORMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT IlwisObjectCreatorModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(QString componentUrl READ componentUrl CONSTANT)
public:
    explicit IlwisObjectCreatorModel(QObject *parent = 0);
    IlwisObjectCreatorModel(const QString& label, quint64 type, const QString& creatorUrl,QObject *parent);
    ~IlwisObjectCreatorModel();

    QString label() const;
    QString componentUrl() const;
    QString type() const;

signals:

public slots:

private:
    QString _label;
    quint64 _type;
    QString _creatorUrl;
};

#endif // IWISOBJECTCREATORMODEL_H
