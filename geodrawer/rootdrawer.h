#ifndef ROOTDRAWER_H
#define ROOTDRAWER_H

#include <memory.h>
#include <QObject>
#include "identity.h"
#include "complexdrawer.h"

namespace Ilwis {
namespace Geodrawer{

class RootDrawer : public ComplexDrawer
{
    Q_OBJECT
public:
    explicit RootDrawer();

    Envelope viewEnvelope() const;

signals:

public slots:

};

typedef std::unique_ptr<RootDrawer> UPRootDrawer;
}
}

#endif // ROOTDRAWER_H
