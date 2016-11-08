#ifndef SMCE_H
#define SMCE_H

#include <QObject>

class SMCE : public QObject
{
    Q_OBJECT
public:
    explicit SMCE(QObject *parent = 0);

signals:

public slots:
};

#endif // SMCE_H