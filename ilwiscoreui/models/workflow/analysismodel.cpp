#include "kernel.h"
#include "ilwisdata.h"
#include "workflow.h"
#include "analysismodel.h"

using namespace Ilwis;

AnalysisModel::AnalysisModel(QObject *parent) : QObject(parent)
{

}

QString AnalysisModel::name() const{
    if ( _analysis)    {
       return _analysis->name();
    }
    return "";

}


