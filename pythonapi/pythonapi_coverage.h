#ifndef PYTHONAPI_COVERAGE_H
#define PYTHONAPI_COVERAGE_H

#include "pythonapi_ilwisobject.h"
#include "pythonapi_coordinatesystem.h"
#include "pythonapi_table.h"
#include "pythonapi_util.h"

#include <vector>

typedef struct _object PyObject;

namespace Ilwis {
    class Coverage;
    typedef IlwisData<Coverage> ICoverage;
}

namespace pythonapi{
    class Domain;
    class ColumnDefinition;
    class Coverage : public IlwisObject{
        public:
            enum AttributeType{atCOVERAGE, atINDEX};
        protected:
            Coverage();
            Coverage(const Ilwis::ICoverage& coverage);
        public:
            virtual ~Coverage(){}
            CoordinateSystem coordinateSystem();
            pythonapi::Envelope envelope();
            void setEnvelope(const pythonapi::Envelope& env);
    };

}

#endif // PYTHONAPI_COVERAGE_H
