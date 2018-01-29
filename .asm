TOKENS
  identifier = ( "_" | "." | letter) { "_" | "."  | letter | digit } .
  gotolabel = ( "_" | "." | letter) { "_" | "."  | letter | digit } CONTEXT (":").
  number = nzdigit { digit }.
  onumber    = "0" { odigit }.
  string = '"' { ('*'  escapeable | strings) } '"' .
  char  = "'" ( '*'  escapeable | chars) "'" .

COMMENTS FROM "/*" TO "*/"
IGNORE '\t' + '\r' + '\n'


PRODUCTIONS
  B =                            
    { Definition }
EOF                              

.
Ident <Name name>
  = identifier                  
  .

  GotoLabel  <Name name>
  =
gotolabel
':'
.

ConstVal
=                              
    number
  | string
  | char
  | onumber
.

Definition
=                              
  SYNC
    ( FunctionDefinition        
  |ArraySize<isarray>            
  ) .

Initializator
=
    (ConstVal
    |Ident<name>
                                
    ).

  ArraySize<bool & isarray>      
    =                            
    ['[' [
  number                        
                                
  ] ']']
[                                
    Initializator                
    {
    ','                          
Initializator                    
} ]
    ';'
  .
FunctionDefinition
=                            
    FunctionHeader
    FunctionBody
.

FunctionHeader
=
  '('  [FormalParamList] ')' .

FunctionBody
= Statement .

FormalParamList
= FormalParameter
    { ',' FormalParameter
} .

FormalParameter
=
  Ident<name>
                                
                                

  .
Statement
= SYNC                      
{ Label                          
| ExtrnDeclaration              
| AutoDeclaration                
}
(  StatementExpression
  | BreakStatement
  | CompoundStatement
  | ContinueStatement
  | GotoStatement
  | IfStatement
  | NullStatement
  | ReturnStatement
  | SwitchStatement
  | WhileStatement
  )                              
                                  
  .

Label
=                                
  "case" ConstVal
  ':'                            
  |  "default" ':'                
  | GotoLabel <name>              
                                  
                                  
                                  
                                  
                                  
                                  

  .

StatementExpression
=
    Expression
    WEAK ';' .

BreakStatement
= "break"                      
                                 
    WEAK ';'
.

CompoundStatement
= "{"                          
  {
    Statement
} "}"                            
  .

ContinueStatement
=
  "continue"                      
  WEAK  ';'
  .

GotoStatement
=
    "goto"
Ident<name>                      
';'
    .

IfStatement
=                              
"if"                              
  '(' Expression
    ')'
    Statement
[ "else" Statement                
    ]                            
.

  NullStatement    = ';'
  .

ReturnStatement
=                            
"return"                         
  [ WEAK  '('
    Expression                    
  WEAK  ')' ]
                                  
';' .

SwitchStatement
=                                
  "switch"
  '(' Expression
  ')'
  CompoundStatement              
  .


WhileStatement
=                              
  "while" '('
          Expression
  ')'
    Statement                    
  .

ExtrnDeclaration
=
  "extrn"
    Ident<name>
                                  

{ ','
Ident<name>
                                  

} ';'  .


AutoDeclaration
=
    "auto"                        
Ident<name>
                                  
[ ('[' number                      
                                  
']'
| number                          
                                  
| onumber                          
                                  

) ]                                
{ ','                              
Ident <name>
                                  

[ ('[' number                      
                                  
']' | number                      
                                  

) ]                                

} ';'  .
Expression
= AssignExpr
  .

AssignExpr
=                                 


  CondExpr

    [(
    "="
    AssignExpr
                                    
                                  
                                  

  | "=*"                     
AssignExpr
                                  
                                  
  | "=/"
    AssignExpr
                
                
  | "=%"
    AssignExpr
                
                
  | "=+"
    AssignExpr
                
                
  | "=-"                                     
    AssignExpr
                
                
  | "=&"                                     
    AssignExpr
                
                
  | "=^"                                     
    AssignExpr
                
                
  | "=|"                                     
    AssignExpr
                
                
  | "=<<"                                     
    AssignExpr
                
                
  | "=>>"                                     
    AssignExpr
                
                
  | "=<"                                     
    AssignExpr
                
                
  | "=<="                                     
    AssignExpr
                
                
  | "=>"                                     
    AssignExpr
                
                
  | "=>="                                     
    AssignExpr
                
                
  | "==="                                     
    AssignExpr
                
                
  | "=!="                                     
    AssignExpr
                
                
    )].

CondExpr
=            
  OrExpr       
['?'                           
  Expression
                             
                             
  ':'                                                                 
  CondExpr                               
                
                
  ].

OrExpr
=            
  XorExpr
  {'|'                                       
XorExpr
                
                
  }.

XorExpr
=              
  AndExpr     
  {'^'                                   
    AndExpr         
                
                
  }.

AndExpr
=              
  EqlExpr     
  {'&'                                       
    EqlExpr                               
                
                
  }.

EqlExpr
=              
  RelExpr
{
        ("=="                                     
        RelExpr
                
                
        |"!="                                     
        RelExpr
                
                
        )
  }.

RelExpr
=              
ShiftExpr
    {
('<'                                         
  ShiftExpr
                
                
|'>'                                         
  ShiftExpr
                
                
|"<="                                       
  ShiftExpr
                
                
|">="                                       
  ShiftExpr
                
                
  )
    }.

ShiftExpr
=              
AddExpr                       
{
  ( "<<"                                     
AddExpr
                
                
  | ">>"                                     
AddExpr
                
                
  )
}.

AddExpr
=            
  MultExpr
{
  ('+'                                         
MultExpr             
                
                
  |'-'                                       
MultExpr                                 
                
                
  )
    }.

MultExpr
=          
UnaryExpr               
    {
( '*'                                       
    UnaryExpr
                
                
| '/'                                       
  UnaryExpr
                
                
| '%'                                       
    UnaryExpr
                
                
  )
    }.

UnaryExpr
=          
    ( PostfixExpr     
    | '&' UnaryExpr
    | '*' UnaryExpr
    | '+' UnaryExpr
    | '-' UnaryExpr
    | '~' UnaryExpr
    | '!' UnaryExpr
    | "++" UnaryExpr
    | "--" UnaryExpr
).

PostfixExpr
=
Primary
{ '[' Expression ']'
| '(' [ArgExprList] ')'
| "++"
| "--" }
.

Primary
=
Ident<name>
| ConstVal
|'(' Expression ')'
.

ArgExprList
=            
  AssignExpr    
                

  {','          
  AssignExpr    
                

  }.

END B.
