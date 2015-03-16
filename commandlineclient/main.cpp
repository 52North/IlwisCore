#include <QCoreApplication>
#include <iostream>
#include "kernel.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "errorobject.h"

int main(int argc, char *argv[])
{
    try{

        QCoreApplication a(argc, argv);

        QString expr;
        bool scriptStatementAtBegin = false;
        for(int i = 0; i < argc;  ++i){
            if ( i >= 2){
                QString value(argv[i]);
                if ( expr != "")
                    expr = expr + " ";
                else
                    scriptStatementAtBegin = value == "script ";

                expr += value;
            }
        }
        if (!scriptStatementAtBegin)
            expr = "script " + expr;
        std::cout << "main 1" << "\n";
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
