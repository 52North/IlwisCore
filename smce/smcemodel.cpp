#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "modeller/applicationmodel.h"
#include "modeller/model.h"
#include "workflow/modelbuilder.h"
#include "smcemodel.h"
#include "smce.h"

REGISTER_ANALYSISMODEL(SMCEModel,"smce")

SMCEModel::SMCEModel()
{

}

SMCEModel::SMCEModel(Ilwis::AnalysisPattern *p) : AnalysisModel(p){
    _panels["main"] = "SMCE/SMCEPanel.qml";
    _panels["form"] = "SMCE/SMCEForm.qml";
}

AnalysisModel *SMCEModel::create(Ilwis::AnalysisPattern *pattern)
{
    return new SMCEModel(pattern);
}

bool SMCEModel::execute(const QVariantMap &inputParameters, QVariantMap &outputParameters)
{

    // execute the tree calculation here:
    // create the python script and run it using "//operations/runpython"
    /*

    QString line = QStringLiteral("import ilwis\nilwis.Engine.setWorkingCatalog('file:///F:/testdata2/BFD/Final_Data/SMCE_Greenbelt_submission_2711/sub')\n") +
                    QStringLiteral("rc1 = ilwis.Engine.do('binarylogicalraster', ilwis.RasterCoverage('Embanked_areas_dist_sub.mpr'), 15232.2175, 'less')\n") +
                           QStringLiteral("rc4 = ilwis.Engine.do('binarymathraster', 0.000012637687191638382, ilwis.RasterCoverage('Embanked_areas_dist_sub.mpr'), 'times')\n") +
                           QStringLiteral("rc2 = ilwis.Engine.do('binarymathraster', rc4, 0.15500000000000003, 'add')\n") +
                           QStringLiteral("rc5 = ilwis.Engine.do('binarymathraster',0,ilwis.RasterCoverage('Embanked_areas_dist_sub.mpr'), 'times')\n") +
                           QStringLiteral("rc3 = ilwis.Engine.do('binarymathraster', rc5, 0.14, 'add')\n") +
                            QStringLiteral("pythonresult = ilwis.Engine.do('binarymathraster', ilwis.Engine.do('binarymathraster', rc1 , rc2, 'times'), ilwis.Engine.do('binarymathraster', ilwis.Engine.do('binarymathraster', 1, rc1, 'subtract'), rc3, 'times'), 'add')\n") +
                            //QStringLiteral("pythonresult.store('pythonresult.mpr', 'map', 'ilwis3')\n");
                            QStringLiteral("pythonresult.store('pythonresult.shp', 'ESRI Shapefile', 'gdal')\n");
                           //QStringLiteral("ilwis.Engine.do('iffraster', rc1, rc2,rc3)\n");


    QString expr = QString("pythonresult=runpython(\"%1\")").arg(line);

    Ilwis::ExecutionContext ctx;
    Ilwis::SymbolTable syms;
    DOTEST(Ilwis::commandhandler()->execute(expr,&ctx,syms), "execution done");

    IRasterCoverage raster = syms.getValue<IRasterCoverage>(ctx._results[0]);

    // todo: convert pythonresult.mpr to .shp format

    */

    if ( _analysis){
        return _analysis->execute(inputParameters, outputParameters);
    }
    return false;
}

Node* SMCEModel::tree() const
{
    return ((Ilwis::Smce::SMCE*)_analysis)->root();
}
