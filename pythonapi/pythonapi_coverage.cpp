#include "../../core/kernel.h"
#include "../../core/ilwisobjects/ilwisdata.h"
#include "../../core/ilwisobjects/ilwisobject.h"
#include "../../core/ilwisobjects/coverage/coverage.h"
#include "../../core/ilwisobjects/domain/domain.h"

#include "../../core/ilwisobjects/table/columndefinition.h"
#include "../../core/ilwisobjects/table/table.h"

#include "pythonapi_domain.h"
#include "pythonapi_coverage.h"
#include "pythonapi_pyobject.h"
#include "pythonapi_util.h"
#include "pythonapi_qvariant.h"
#include "pythonapi_columndefinition.h"

using namespace pythonapi;

Coverage::Coverage():IlwisObject(){}

Coverage::Coverage(const Ilwis::ICoverage &coverage):IlwisObject(new Ilwis::IIlwisObject(coverage)){
}

CoordinateSystem Coverage::coordinateSystem(){
    return CoordinateSystem(Ilwis::ICoordinateSystem(this->ptr()->as<Ilwis::Coverage>()->coordinateSystem()));
}

Envelope Coverage::envelope(){
    return Envelope(this->ptr()->as<Ilwis::Coverage>()->envelope());
}

void Coverage::setEnvelope(const Envelope &env){
    this->ptr()->as<Ilwis::Coverage>()->envelope(env.data());
}


