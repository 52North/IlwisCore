#ifndef DESKTOPTRANQUILIZER_H
#define DESKTOPTRANQUILIZER_H

#include "tranquilizer.h"

namespace Ilwis {
namespace Geodrawer {

class DesktopTranquilizer : public BaseTranquilizer
{
    Q_OBJECT
public:
    DesktopTranquilizer(const IOOptions& opt, QObject *parent);
    ~DesktopTranquilizer();

    void prepare(const QString &title, const QString &description, double end, double start=0);
    bool update(double step);
    void stop();

    static Tranquilizer *create(const IOOptions &opt);

private:
    bool _runsInMainThread = true;

};
}
}

#endif // DESKTOPTRANQUILIZER_H
