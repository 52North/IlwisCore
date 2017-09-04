#ifndef PYTHONAPI_EXTENSION_H
#define PYTHONAPI_EXTENSION_H

#include "pythonapi_qtGNUTypedefs.h"

#include "../../core/ilwistypes.h"

namespace pythonapi{
    //init modul
    bool _initIlwisObjects(const char * ilwisDir);
    void _exitIlwisObjects();
    void disconnectIssueLogger();
    void connectIssueLogger();

    //ilwisTypes
    struct it{
        static const quint64 FEATURE = itFEATURE;
        static const quint64 RASTER = itRASTER;
        static const quint64 COVERAGE = itCOVERAGE;
        static const quint64 TABLE = itTABLE;
        static const quint64 GEOREF = itGEOREF;
        static const quint64 COORDSYSTEM = itCOORDSYSTEM;
        static const quint64 ILWDOMAIN = itDOMAIN;
        static const quint64 NUMERICDOMAIN = itNUMERICDOMAIN;
        static const quint64 ITEMDOMAIN = itITEMDOMAIN;
        static const quint64 SINGLEOPERATION = itSINGLEOPERATION;
        static const quint64 WORKFLOW = itWORKFLOW;
        static const quint64 PROJECTION = itPROJECTION;
        static const quint64 ELLIPSOID = itELLIPSOID;
        static const quint64 CATALOG = itCATALOG;
        static const quint64 CONTINUOUSCOLOR = itCONTINUOUSCOLOR;
        static const quint64 PALETTECOLOR = itPALETTECOLOR;
        static const quint64 VALUERANGE = itVALUERANGE;
        static const quint64 COLLECTION = itCOLLECTION;
    };
}
#endif // PYTHONAPI_EXTENSION_H
