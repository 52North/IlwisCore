#include <QKeyEvent>
#include "keyfilter.h"

KeyFilter::KeyFilter(QObject *parent) :
    QObject(parent)
{
}

bool KeyFilter::eventFilter(QObject *object, QEvent *event){
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        return false;
    }

    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        return false;
    }

    if(event->type() == QEvent::Shortcut){
        QShortcutEvent *sc = static_cast<QShortcutEvent *>(event);
        const QKeySequence &ks = sc->key();

        return false;
    }

    return false;
}
