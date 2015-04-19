#ifndef COLUMNMODEL_H
#define COLUMNMODEL_H

#include <QObject>

class ColumnModel : public QObject
{
    Q_OBJECT
public:
    explicit ColumnModel(QObject *parent = 0);
    ~ColumnModel();

signals:

public slots:
};

#endif // COLUMNMODEL_H
