#include <QCoreApplication>
#include <iostream>
#include "kernel.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "raster.h"
#include "errorobject.h"

int main(int argc, char *argv[])
{
    try{
        QCoreApplication a(argc, argv);

        bool scriptStatementAtBegin = false;

#ifdef QT_DEBUG
        //QString expr = "aaas{format(map,'ilwis3')}=linearrasterfilter(file:///d:/projects/ilwis/ilwis4/testdata/small21.mpr,\"code=0.5 0 0.5 0 1 0 0.5 0 0.5\");";
        QString expr = "runpython(\"import ilwis\")";
#else
        QString expr;
        // expected:
        //      ilwis -c command params
        // so skip first 2 params (ilwis, -c)
        for(int i = 2; i < argc;  ++i){
            QString value(argv[i]);
            if ( expr != "")
                expr = expr + " ";
            else
                scriptStatementAtBegin = value == "script ";

            expr += value;
        }
#endif

        if (!scriptStatementAtBegin)
            expr = "script " + expr;

        Ilwis::initIlwis(Ilwis::rmCOMMANDLINE | Ilwis::rmNOUI);
        Ilwis::ExecutionContext ctx;
        Ilwis::SymbolTable syms;
        Ilwis::commandhandler()->execute(expr,&ctx,syms);

        Ilwis::exitIlwis();
        return 0;

    } catch(const Ilwis::ErrorObject& err){
        std::cerr << err.message().toStdString();

    }
    catch(std::exception& ex){
        std::cerr << ex.what();
    }
    catch(...){
        std::cerr << "unknown error";
    }
    return 1;
}
