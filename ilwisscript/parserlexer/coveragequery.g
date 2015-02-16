grammar coveragequery;

spatialquery returns [ ASTNode *node] 
	:	expression				
	;
	
term returns [ TermNode *node]
	:	'(' expression ')'			
	|	INT						
	|	FLOAT						
	|	STRING
	|       ID	
	|	ID '(' 	(parameters)? ')'	
	|	ID '(' coordinatespart ')'
	|	ID '->' ID						
	;
	
coordinatespart
	:  coordinates+
	;
	
coordinates
	: '(' coordinateptuple+ ')'
	;
	
coordinateptuple
	: FLOAT FLOAT
	| INT INT
	;
		 	
parameters returns [ ParametersNode *node]
	:	expression 
		(
		',' 
		expression
		)*
	;	

negation  returns [ TermNode *node]
	:	('not' 					
		)*
		term					
	;

unary  returns [ TermNode *node]

	:	('+' 					
		| '-'					
		)? 
		negation				
	;
mult  returns [ MultiplicationNode *node]
	:	un1=unary 				
		(
		('*' 					
		| '/'					
		| 'mod'					
		) 
		unary				
		)*
	;
	
add  returns [ AddNode *node]
	:	mult 		
		(
		('+'				
		| 
		'-'					
		) 	
		mult			
		)*
	;
	
relation  returns [ RelationNode *node]
	:	add				
		(
		(
		'==' 					
		| '!=' 					
		| '<' 					
		| '<=' 					
		| '>=' 					
		| '>'					
		) 
		add				
		)*
	;
	
expression returns [ ExpressionNode *node]
	:	relation			
		(				
		( 'and'  				
		| 'or'  				
		) relation		
		) *
	;		

	
// LEXER---------------------------------------------------------------------
//---------------------------------------------------------------------------
ID  :	('a'..'z'|'A'..'Z'|'_'|'..') ('a'..'z'|'A'..'Z'|'0'..'9'|'_'|'.'|':'| '/')*
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
    ;

fragment
EXPONENT : ('e'|'E') ('+'|'-')? ('0'..'9')+ ;


fragment
ESC_SEQ
    :   '\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\')
    ;

EOL 	: ('\r' | '\n')+;
