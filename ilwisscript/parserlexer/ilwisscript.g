grammar ilwisscript;

options {
  language = C;

}

@preincludes{
#include <QString>
#include <QStringList>
#include <QHash>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "scriptnode.h"
#include "operationnodescript.h"
#include "expressionnode.h"
#include "outparametersnode.h"
#include "assignmentnode.h"
#include "scriptlinenode.h"
#include "parametersNode.h"
#include "selectornode.h"
#include "geos/geom/Coordinate.h"
#include "ilwiscoordinate.h"
#include "selectnode.h"
#include "termnode.h"
#include "variableNode.h"
#include "returnnode.h"
#include "functionnode.h"
#include "multiplicationnode.h"
#include "addnode.h"
#include "relationnode.h"
#include "commandnode.h"
#include "whilenode.h"
#include "breaknode.h"
#include "valuerangenode.h"
#include "domainformatter.h"
#include "ifnode.h"
#include "formatter.h"
#include "functionstatementnode.h"
 
using namespace Ilwis;
}


script returns [ ASTNode *node]
@init{
	node = new ScriptNode();
}
	:	( scriptLine  {node->addChild($scriptLine.node); }  )+
	;
	
scriptLine returns [ ASTNode *node] 
@init{
	node =  new ScriptLineNode();
}
	:	statement				 { node->addChild($statement.node); } 	';'
	| 	EOL
	;
	
statement returns [ ASTNode *node]
	:	assignmentStatement				{ node = $assignmentStatement.node; }
	|	ifStatement					{ node = $ifStatement.node; }
	|	whileStatement					{ node = $whileStatement.node; }
	|	functionStatement				{ node = $functionStatement.node; }
	|	commandStatement				{ node = $commandStatement.node; }
	|	dataFormatter					{ node = $dataFormatter.node; }
	|	defineStatement
	;
	
selectExpr returns [SelectNode *node]
@init {
  node = new SelectNode();
} 
	:      'indexes' 'from' sources 'where' expr=expression {  node->setInput($sources.source);
								 node->setExpression($expr.node);
									    }
	;

sources returns [ QString source]
	:	STRING					{ source = "url|" + QString((char *)($STRING.text->chars));}
	|	ID					{ source = "id|" + QString((char *)($ID.text->chars));}
	;	

defineStatement
	:	'define' ID '=' STRING
	;

	
assignmentStatement returns [ AssignmentNode *node]
@init{
	node= new AssignmentNode();
}
	:	outParameters					{ node->addOutputs($outParameters.node); }
		'='					{ node->setDefintion(true); } 
		expression 					{ node->setExpression($expression.node);}
        |       id1=ID '='						{ node->setOutId(new IDNode((char *)($id1.text->chars)));}
                selectExpr					{ node->setExpression($selectExpr.node);}
	;
	
outParameters returns [ OutParametersNode *node]
@init{
	node= new OutParametersNode();
}
	: id1=ID (sel1=selector					{ node->addSelector((char *)($id1.text->chars),$sel1.node);}		)?						
								{ node->addResult(new IDNode((char *)($id1.text->chars))); }
		(for1=formatPart 				{ node->addSpecifier((char *)($id1.text->chars), $for1.node); }	)?
	(',' id2=ID (sel2=selector				{ node->addSelector((char *)($id2.text->chars), $sel2.node);}		)?						
								{ node->addResult(new IDNode((char *)($id2.text->chars))); }
		(for2=formatPart 				{ node->addSpecifier((char *)($id2.text->chars), $for2.node); }	)?
	)*		
	;			

actualParameters returns [ ParametersNode *node]
@init{
	node = new ParametersNode();
}
	:	expr1 = expression 				{ node->addChild($expr1.node); }
		(
		',' 
		expr2 = expression				{ node->addChild($expr2.node); }
		)*
	;
	
term returns [ TermNode *node]
@init{
	node = new TermNode();
}
	:	'(' expression ')'				{ node->setExpression($expression.node); }	
	|	INT						{ node->setNumerical((char *)($INT.text->chars)); }
	|	FLOAT						{ node->setNumerical((char *)($FLOAT.text->chars)); }
	|	STRING						{ node->setString((char *)($STRING.text->chars)); }
	|	id1 = ID '(' 					{ node->setId(new IDNode((char *)($id1.text->chars))); }
		(actualParameters				{ node->setParameters($actualParameters.node); }
		) ')'
	|	id4 = ID '(' 					{ node->setId(new IDNode((char *)($id4.text->chars))); }
								{ node->setParameters(); }
		 ')'									
	|	id2 = ID 					{ node->setId(new IDNode((char *)($id2.text->chars)));}
		(id3=selector					{node->addSelector($id3.node);})+
	|	id2 = ID					{ node->setId(new IDNode((char *)($id2.text->chars)));}
	;
	
selector returns [ Selector *node]
@init{
	node = new Selector("selector");
}
	: '[' id1=INT id2=INT ',' id3=INT id4=INT ']'		{ node->setSelectorType("box"); node->setBounds((char *)($id1.text->chars),
									(char *)($id2.text->chars),
									(char *)($id3.text->chars),
									(char *)($id4.text->chars)); }
	| '[' id1=ID (',' 'key=' id2=ID )? ']'			{ node->setSelectorType("var"); 
								node->setVariable((char *)($id1.text->chars));
								if ( id2 != 0) node->keyColumns((char *)$id2.text->chars);}
	| '[' INT ']'						{ node->setSelectorType("index"); node->setVariable((char *)($INT.text->chars));}
	| '[' id1=ID  id2=ID (',''key=' id3=ID )?']'		{ node->setSelectorType("columnrange");
								node->beginCol((char *)$id1.text->chars); 
								node->endCol((char *)$id2.text->chars); 
								if ( id3 != 0) node->keyColumns((char *)$id3.text->chars);} 
	| '[' id1=INT  id2=INT ']'				{ node->setSelectorType("recordrange"); 
								node->beginRec((char *)$id1.text->chars); 
								node->endRec((char *)$id2.text->chars); }
	| '[' id1=ID id2=ID ',' id3=INT id4=INT (',' 'key=' id5=ID )? ']'		{ node->setSelectorType("columnrecordrange");
								node->beginCol((char *)$id1.text->chars); 
								node->endCol((char *)$id2.text->chars); 
								node->beginRec((char *)$id3.text->chars); 
								node->endRec((char *)$id4.text->chars);
								if ( id5 != 0) node->keyColumns((char *)$id5.text->chars);} 
	;

negation  returns [ TermNode *node]
@init{
	bool negated = false;
}
	:	('not' 					{ negated = !negated; }
		)*
		term					{ $term.node->setLogicalNegation(negated); node = $term.node;}
	;

unary  returns [ TermNode *node]
@init{
	bool isnegated = false;
}

	:	('+' 					
		| '-'					{ isnegated = !isnegated; }
		)? 
		negation				{ $negation.node->setNumericalNegation(isnegated); node = $negation.node; }
	;
mult  returns [ MultiplicationNode *node]
@init{
	node = new MultiplicationNode();
	OperationNodeScript::Operators op;
}
	:	un1=unary 				{ node->setLeftTerm(un1); }
		(
		('*' 					{ op = OperationNodeScript::oTIMES; }
		| '/'					{ op = OperationNodeScript::oDIVIDED; }
		| 'mod'					{ op = OperationNodeScript::oMOD; }
		) 
		un2=unary				{ node->addRightTerm(op, un2); }
		)*
	;
	
add  returns [ AddNode *node]
@init{
	node = new AddNode();
	OperationNodeScript::Operators op;
}
	:	mul1 = mult 				{ node->setLeftTerm(mul1); }
		(
		('+'					{ op = OperationNodeScript::oADD; } 
		| 
		'-'					{ op = OperationNodeScript::oSUBSTRACT; }
		) 	
		mul2 = mult				{ node->addRightTerm(op, mul2); }
		)*
	;
	
relation  returns [ RelationNode *node]
@init{
	node = new RelationNode();
	OperationNodeScript::Operators op;
}
	:	add1=add				{ node->setLeftTerm(add1); } 
		(
		(
		'==' 					{ op = OperationNodeScript::oEQ; } 
		| '!=' 					{ op = OperationNodeScript::oNEQ; } 
		| '<' 					{ op = OperationNodeScript::oLESS; } 
		| '<=' 					{ op = OperationNodeScript::oLESSEQ; } 
		| '>=' 					{ op = OperationNodeScript::oGREATEREQ; } 
		| '>'					{ op = OperationNodeScript::oGREATER; } 
		) 
		add2=add				{ node->addRightTerm(op, add2); }
		)*
	;
	
expression returns [ ExpressionNode *node]
@init{
	node = new ExpressionNode();
	OperationNodeScript::Operators op;
}
	:	rel1 = relation				{ node->setLeftTerm(rel1); } 
		(				
		( 'and'  				{ op = OperationNodeScript::oAND; } 
		| 'or'  				{ op = OperationNodeScript::oOR; } 
		| 'xor' 				{ op = OperationNodeScript::oXOR; } 
		) rel2 = relation			{ node->addRightTerm(op, rel2); }
		) *
	;		

commandStatement returns [ CommandNode *node]
@init{
	node = new CommandNode();
	QString flag;
	ExpressionNode *expr;
}
	:	
	id1 = ID					{ node->setCommand((char *)($id1.text->chars)); }						 
	(
	'--' 
	id2 = ID 					{ flag = QString((char *)($id2.text->chars));}
	('='
	expression					{ expr = $expression.node; }
	)? 						{ node->addOption(flag, expr); expr = 0;}
	)*
	;	
	
functionStatement returns [ FunctionStatementNode *node]
	:	ID					{ node = new FunctionStatementNode((char *)($ID.text->chars)); } 
	'(' actualParameters? 				{ node->setParameters($actualParameters.node); }
	')' 
	;	
	
whileStatement returns [ WhileNode *node]
@init{
	node = new WhileNode();
}
	:	'while' expression 'do' (EOL)*	{ node->setCondition($expression.node); }
		( (statement 				{ node->addChild($statement.node); }
		|				
		breakStatement  			{ node->addChild($breakStatement.node); }
		) (EOL)*	 
		)*
		'endwhile' 
	;
	
breakStatement returns [ BreakNode *node]
@init{
	node = new BreakNode();
}
	:	'break' 'when' expression		{ node->addChild($expression.node); } 
	;		

formatPart returns [ ASTNode *node]

	:	'{' formatters '}'			{ node=$formatters.node;}
	;

formatters returns [ ASTNode *node ]
@init{
	node = new ASTNode("Formatters");
}
	: formatter					{ node->addChild($formatter.node); }
	| formatter ';' formatters			{ node->addChild($formatter.node); }
	;

formatter returns [ ASTNode *node]
	
	: dataFormatter				{ node = $dataFormatter.node;}
	| domainFormatter			{ node = $domainFormatter.node;}
	| grouper
	| reintepreter
	;

grouper	: 'groupBy' '(' idlist')'
	;

reintepreter
	: 'reinterpret(' idlist '=' ID ',' STRING ',' STRING ')'
	| 'reinterpret(' STRING ')'
	;
	
idlist	: ID+
	
	;

dataFormatter returns [ Formatter *node]
@init{
	node = new Formatter();
}	
	: 'format' '(' ID ',' STRING ',' id1=datatype ')'{ node->setDataType($id1.typeName);
							  node->setFormatNameSpace(new IDNode((char *)($ID.text->chars)));
							  node->setFormat((char *)($STRING.text->chars));}	
	| 'format' '(' ID ',' STRING ')'			{ node->setFormatNameSpace(new IDNode((char *)($ID.text->chars)));
							  node->setFormat((char *)($STRING.text->chars));}
	| 'format' '(' STRING ')'			{ node->setFormat((char *)($STRING.text->chars)); }
	;

datatype returns [ QString typeName]
@init{
}
	: 'rastercoverage'				{ typeName = "gridcoverage"; }
	| 'polygoncoverage'				{ typeName = "polygoncoverage"; }
	| 'linecoverage'				{ typeName = "linecoverage";}
	| 'pointcoverage'				{ typeName = "pointcoverage"; }
	| 'featurecoverage'                             { typeName = "featurecoverage";}
	| 'feature-index'                               { typeName = "feature-index";}
	| 'table'					{ typeName = "table"; }	
	;
	
domainFormatter returns [ DomainFormatter *node]
@init{
	node = new DomainFormatter();
}
	:	'dom' '=' ID 			{ node->setDomainId(new IDNode((char *)($ID.text->chars)));}
	|	'dom' '=' valrangePart 		{ node->setValueRange($valrangePart.node);}
	;	

valrangePart returns [ ValueRangeNode *node]
	:	'vr' '=' 
	;	
	
ifStatement returns [ Ifnode *node]
@init{
	node = new Ifnode();
}
	:	'if' expression (EOL)? 'then' EOL* (result=statement {node->addThen($result.node);} EOL* )+ { node->setCondition($expression.node);  } 
		('else' EOL* (result2=statement  {node->addElse($result2.node ); } EOL*)+)?			
		'endif'
	
	;
returnStatement returns [ ReturnNode *node]
@init{
	node = new ReturnNode();
}
	:	'return' expression?			{node->addChild($expression.node); } 
	;
		
	
	
// LEXER---------------------------------------------------------------------
//---------------------------------------------------------------------------
ID  :	('a'..'z'|'A'..'Z'|'_'|'..'|'?') ('a'..'z'|'A'..'Z'|'0'..'9'|'_'|'.'|':'| '/'|'?')*
    ;

INT :	'0'..'9'+
    ;

FLOAT
    :   ('0'..'9')+ '.' ('0'..'9')* EXPONENT?
    |   '.' ('0'..'9')+ EXPONENT?
    |   ('0'..'9')+ EXPONENT
    ;

COMMENT
    :   '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    |   '/*' ( options {greedy=false;} : . )* '*/' {$channel=HIDDEN;}
    |	'Rem' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    ;

WS  :   ( ' '
        | '\t'
        | '\r'
        ) {$channel=HIDDEN;}
    ;

STRING
    :  '"' ( ~('"') )* '"'
    |  '\''( ~('\'') )* '\'' 
    ;

fragment
EXPONENT : ('e'|'E') ('+'|'-')? ('0'..'9')+ ;


fragment
ESC_SEQ
    :   '\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\')
    ;

EOL 	: ('\r' | '\n')+;

