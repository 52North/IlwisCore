#ifndef PREFERENCESMODEL_H
#define PREFERENCESMODEL_H

#include <QObject>

class PreferencesModel : public QObject
{
    Q_OBJECT
public:
    explicit PreferencesModel(QObject *parent = 0);

signals:

public slots:
};

#endif // PREFERENCESMODEL_H
