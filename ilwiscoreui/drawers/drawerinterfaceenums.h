#ifndef DRAWERINTERFACEENUMS_H
#define DRAWERINTERFACEENUMS_H

#include <memory>
#include <QColor>
#include <GL/gl.h>
#include <QVector3D>
#include "ilwiscoreui_global.h"
#include <math.h>
#include "kernel.h"
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
#include "location.h"
#include "ilwisdata.h"
#include "representation.h"
#include "identityinterface.h"
//#include "complexdrawer.h"
#include "iooptions.h"

class QOpenGLContext;

namespace Ilwis {

namespace Geodrawer{

class ILWISCOREUISHARED_EXPORT DrawerInterface {
public:
    enum PreparationType{ptNONE=0,ptRENDER=1,ptGEOMETRY=2,ptINITOPENGL=4,ptUI=8,pt3D=16,ptANIMATION=32,
                         ptRESTORE=64,ptOFFSCREENSTART=128,ptOFFSCREENEND=256,
                         ptREDRAW=512,ptNEWCSY=1024,ptMVP=2048,ptSHADERS=4096, ptRESET=8192,ptALL=4294967295};
    enum ColorValueMeaning{cvmFRACTION, cvmTRUEVALUE};
    enum DrawerType{dtDONTCARE=0, dtPOST=1, dtPRE=2, dtMAIN=4, dtALL=0xFFFFFFFF};
};

}

}
#endif // DRAWERINTERFACEENUMS_H
