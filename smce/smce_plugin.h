#ifndef GEODRAWER_PLUGIN_H
#define GEODRAWER_PLUGIN_H

#include <QQmlExtensionPlugin>

class SMCEPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // GEODRAWER_PLUGIN_H

