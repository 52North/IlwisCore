#include <iostream>
#include <fstream>
#include <QUrl>
#include <QFileInfo>
#include "kernel.h"
#include "ilwisdata.h"
#include "resource.h"
#include "identity.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "script.h"
#include "parserlexer/IlwisScriptLexer.h"
#include "parserlexer/IlwisScriptParser.h"
#include "symboltable.h"


using namespace Ilwis;
OperationImplementation *Script::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Script(metaid, expr);
}

Script::Script()
{
}

Script::Script(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

OperationImplementation::State Script::prepare() {
    QString txt = _expression.parm(0).value();
    QUrl url(txt);
    if ( url.isValid() && url.scheme() == "file") {
        QFileInfo inf( url.toLocalFile());
        bool exists = inf.exists();
        if (exists && inf.suffix() == "isf") {
            std::string text;
            std::ifstream in(inf.absoluteFilePath().toLatin1(), std::ios_base::in);
            if(in.is_open() && in.good()) {
                while(!in.eof()) {
                    std::string line;
                    std::getline(in, line);
                    text += line + ";";
                }
                char *buf = new char[text.size()];
                memcpy(buf,text.c_str(), text.size());
                _buffer.reset( buf );
                _bufferSize = text.size();
                return sPREPARED;
            }
        } else {
            return sPREPAREFAILED;
        }
    } else {
        return sPREPARED;
    }

    return sNOTPREPARED;
}

bool Script::execute(ExecutionContext *ctx)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare()) != sPREPARED)
            return false;

    ANTLR3_UINT8 * bufferData = (ANTLR3_UINT8 *) _buffer.get();
    for(int i=0; i < _bufferSize; ++i) {
        if ( bufferData[i] == '\n') { //replace newlines with ; which are the actual seperators
            bufferData[i]=';';
        }
    }
    if ( bufferData[_bufferSize - 1] != ';')
        bufferData[_bufferSize - 1] = ';';

    pANTLR3_INPUT_STREAM input = antlr3StringStreamNew(bufferData,  ANTLR3_ENC_8BIT,  _bufferSize, (pANTLR3_UINT8)"ScriptText");
    if(input == NULL)
        return false;

    pilwisscriptLexer lxr = ilwisscriptLexerNew(input);
    if(lxr == NULL)
        return false;

    //Creates an empty token stream.
    pANTLR3_COMMON_TOKEN_STREAM tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));
    if(tstream == NULL)
        return false;

    //Creates a parser.
    pilwisscriptParser psr = ilwisscriptParserNew(tstream);
    if(psr == NULL)
        return false;

    //Run the parser rule. This also runs the lexer to create the token stream.
    ASTNode *scr = psr->script(psr);
    SymbolTable symbols;
    bool ok = scr->evaluate(symbols, 1000);
    return ok;

}

quint64 Script::createMetadata()
{
    QString urlTxt = QString("ilwis://operations/script");
    QUrl url(urlTxt);
    Resource res(url, itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","ilwisscript");
    res.addProperty("syntax","script file|script scriptline(,scriptline)*");
    res.addProperty("inparameters","1");
    res.addProperty("pin_1_type", itFILE | itSTRING);
    res.addProperty("pin_1_name", TR("input script file"));
    res.addProperty("pin_1_domain","none");
    res.addProperty("pin_1_desc",TR("input file containing script commands"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itBOOL);
    res.addProperty("pout_1_name", TR("succes"));
    res.addProperty("pout_1_domain","bool");
    res.addProperty("pout_1_desc",TR("returns the succes of the execution of the script"));

    res.prepare();
    urlTxt += "=" + QString::number(res.id());
    res.setUrl(QUrl(urlTxt));
//    IOperationMetaData md;
//    if(!md.prepare(res)) {
//        return i64UNDEF;
//    }
    mastercatalog()->addItems({res});
    return res.id();
}
