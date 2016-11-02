#ifndef DRAWERCOLORSETS_H
#define DRAWERCOLORSETS_H

#include <QVariant>
#include <QColor>
#include <QString>


namespace Ilwis{

namespace Geodrawer{


struct Colorset {
    QColor area;
    QColor border;
} ;


class DrawerColorsets
{
private:
    QHash<QString, Colorset> _colorsets;

    Colorset GetColorset(const QString &colorset) const;

public:
    DrawerColorsets();

    Colorset operator [](const QString& key) const;
};


}

}
#endif // DRAWERCOLORSETS_H
