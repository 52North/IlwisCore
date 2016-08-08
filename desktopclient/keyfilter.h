#ifndef KEYFILTER_H
#define KEYFILTER_H

#include <QObject>

class KeyFilter : public QObject
{
    Q_OBJECT
public:
    explicit KeyFilter(QObject *parent = 0);

    bool eventFilter(QObject *object, QEvent *event);
signals:
    void keyPressed(int key);
    void keyReleased(int key);
public slots:

};

#endif // KEYFILTER_H
