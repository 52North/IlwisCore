#include <iostream>
#include <fstream>
#include <QUrl>
#include <QFileInfo>
#include "kernel.h"
#include "ilwisdata.h"
#include "resource.h"
#include "identity.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "script.h"
#include "parserlexer/ilwisscriptParser.h"
#include "parserlexer/IlwisScriptLexer.h"

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

bool Script::detectKey(const std::string& line, const std::string& key) {
    int index = line.find(key);
    if ( index == std::string::npos)
        return false;
    if ( index == 0)
        return true;

    int index1 = line.find('"');
    int index2 = line.find_last_of('"');
    bool ok = index < index1 || index > index2;
    if (!ok)
        return false;

    if ( index != 0 && line[index -1] == ' ')
        return true;
    return false;
}

OperationImplementation::State Script::prepare(ExecutionContext *, const SymbolTable&) {
    QString txt = _expression.parm(0).value();
    if ( txt == "") {
        ERROR2(ERR_NOT_FOUND2, TR("valid expression"), _expression.toString());
        return sPREPAREFAILED;
    }
    QUrl url(txt);
    QFileInfo inf( url.toLocalFile());
    bool exists = inf.exists();
    if (  exists) {

        std::string text;
        std::ifstream in(inf.absoluteFilePath().toLatin1(), std::ios_base::in);
        int ignorenCount=0;
        if(in.is_open() && in.good()) {
            while(!in.eof()) {
                std::string line;
                std::getline(in, line);
                if ( line == "") // skip empty lines
                    continue;
                if (detectKey(line, "if") || detectKey(line, "while") ){
                    ignorenCount++;
                }
                if (detectKey(line, "endif") || detectKey(line, "endwhile")) {
                    ignorenCount--;
                }
                text += line + (ignorenCount != 0 ? " " : ";");
            }
            char *buf = new char[text.size()];
            memcpy(buf,text.c_str(), text.size());
            _buffer.reset( buf );
            _bufferSize = text.size();
            return sPREPARED;
        }

    } else {
        QString text = txt.trimmed();
        if ( text[text.size() - 1] != ';')
            text += ';';
        char *buf = new char[text.size()];
        memcpy(buf,text.toLatin1(), text.size());
        _buffer.reset( buf );
        _bufferSize = text.size();
        return sPREPARED;
    }

    return sNOTPREPARED;
}

bool Script::execute(ExecutionContext *ctx, SymbolTable& symbols )
{
    try{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symbols)) != sPREPARED)
            return false;

    ANTLR3_UINT8 * bufferData = (ANTLR3_UINT8 *) _buffer.get();

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

    bool ok = scr->evaluate(symbols, 1000, ctx);
   // logOperation(_expression);
    return ok;
    }
    catch(Ilwis::ScriptError& err) {
        kernel()->issues()->log(err.message());
    }
    return false;

}

quint64 Script::createMetadata()
{
    QString urlTxt = QString("ilwis://operations/script");
    QUrl url(urlTxt);
    Resource resource(url, itSINGLEOPERATION);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","ilwisscript");
    resource.addProperty("syntax","script file|script scriptline(,scriptline)*");
    resource.addProperty("inparameters","1");
    resource.addProperty("pin_1_type", itFILE | itSTRING);
    resource.addProperty("pin_1_name", TR("input script file"));
    resource.addProperty("pin_1_domain","none");
    resource.addProperty("pin_1_desc",TR("input file containing script commands"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itBOOL);
    resource.addProperty("pout_1_name", TR("succes"));
    resource.addProperty("pout_1_domain","bool");
    resource.addProperty("pout_1_desc",TR("returns the succes of the execution of the script"));
    resource.addProperty("keyword","internal");

    resource.prepare();
    urlTxt += "=" + QString::number(resource.id());
    resource.setUrl(QUrl(urlTxt));
//    IOperationMetaData md;
//    if(!md.prepare(resource)) {
//        return i64UNDEF;
//    }
    mastercatalog()->addItems({resource});
    return resource.id();
}
